#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <patricia/patricia.h>

#define SMACQ_MODULE_IS_VECTOR 1
#define SMACQ_MODULE_IS_STATELESS 1
#include <smacq.h>

SMACQ_MODULE(iplookup,
  PROTO_CTOR(iplookup);
  PROTO_DTOR(iplookup);
  PROTO_CONSUME();
  PROTO_PRODUCE();

  int demux;
  struct batch * batch;
  int num_batches;
  int ipaddr_type;

  void add_entry(char * field, char * needle, int output);
  struct batch * get_batch(char * field);
);

struct batch {
  patricia_tree_t * trie;
  DtsField field;
  char * fieldname;
};

static struct smacq_options options[] = {
  {"f", {string_t:NULL}, "Field to inspect (full data is default)", SMACQ_OPT_TYPE_STRING},
  {"m", {boolean_t:0}, "OR multiple fields and demux to individual outputs", SMACQ_OPT_TYPE_BOOLEAN},
  END_SMACQ_OPTIONS
};

struct batch * iplookupModule::get_batch(char * field) {
	struct batch * mybatch;
	int i;
	DtsField f = dts->requirefield(field);

	for (i = 0; i < num_batches; i++) {
		if (dts_comparefields(f, batch[i].field)) {
			return &batch[i];
		}
	}

	num_batches++;
	batch = (struct batch*)realloc(batch, num_batches * sizeof(struct batch));
	mybatch = &batch[num_batches-1];

 	mybatch->field = dts->requirefield(field);
	mybatch->fieldname = field;
  	mybatch->trie = New_Patricia(32);

	return mybatch;
}

void iplookupModule::add_entry(char * field, char * needle, int output) {
  struct batch * mybatch = NULL;
  prefix_t * pfx;

  if (!field && !needle) return;

  if (!needle) {
  	needle = field;
	field = NULL;
  }

  if (field) {
	mybatch = get_batch(field);
  }

  pfx = ascii2prefix(AF_INET, needle);
  if (!pfx) {
 	fprintf(stderr, "iplookup: unable to parse mask: %s\n", needle);
  	assert(pfx);
  }
  patricia_node_t * node = patricia_lookup(mybatch->trie, pfx);
  /* fprintf(stderr, "%s mybatch->trie is %p, output is %d, node is %p\n", needle, mybatch, output, node); */
    
  assert(node);
  if (node->data) {
  	assert(!"duplicate mask");
  	//deref_data(node->data);
  }
  node->data = (void*)output;
}

smacq_result iplookupModule::produce(DtsObject & datum, int & outchan) {
  return dequeue(datum, outchan);
}

smacq_result iplookupModule::consume(DtsObject datum, int & outchan) {
  DtsObject field;
  int matched = 0;
  int i;
  patricia_node_t * node;

  assert(datum);

  for (i=0; i < num_batches; i++) {
     struct batch * mybatch = &batch[i];
     prefix_t * prefix;

     if (mybatch->field) {
  	field = datum->getfield(mybatch->field);
  	if (!field) return SMACQ_FREE;
     } else {
        field = datum;
     }

     assert(field->gettype() == ipaddr_type);
     prefix = New_Prefix(AF_INET, field->getdata(), 32);
 
     node = patricia_search_best(mybatch->trie, prefix);
     Deref_Prefix(prefix);	 
     if (node) {
	  	outchan = (int)node->data;
		if (matched) {
			enqueue(datum, outchan);
			
	  	} else {
	  		matched = 1;
		} 
     }
  }

  if (matched) {
	return (SMACQ_PASS|canproduce());
  } else {
  	return SMACQ_FREE;
  }

}

iplookupModule::iplookupModule(struct SmacqModule::smacq_init * context) 
 	: SmacqModule(context), num_batches(0)
{
  int i, argc;
  char ** argv;
  smacq_opt field_opt, demux_opt;
  char * field = NULL;
  char * pattern = NULL;
  int output = 0;

  batch = (struct batch*)calloc(sizeof(struct batch), 1);

  {
    struct smacq_optval optvals[] = {
      {"f", &field_opt},
      {"m", &demux_opt},
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &argc, &argv,
				 options, optvals);

    demux = demux_opt.boolean_t;
  }

  ipaddr_type = dts->requiretype("ip");

  for (i = 0; i < argc; i++) {
	  if (!strcmp(argv[i], ";") && pattern) {
		if (!field) field = field_opt.string_t; 
		//fprintf(stderr, "middle pattern is %s\n", pattern);
		add_entry(field, pattern, output++);
		field = NULL;
		pattern = NULL;
	  } else if (!field) {
		field = argv[i];
	  } else {
		pattern = argv[i];
	  }
  }
  if (pattern) {
	if (!field) field = field_opt.string_t; 
	//fprintf(stderr, "last pattern of %p is %s\n", context->self, pattern);
  	add_entry(field, pattern, output++);
  }

  if (output>1) {
	  demux = 1;
  }
}

iplookupModule::~iplookupModule() {
  int i;

  for (i=0; i < num_batches; i++) {
  	Destroy_Patricia(batch[i].trie, NULL);
  }
}

