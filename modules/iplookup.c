#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <smacq.h>
#include <patricia/patricia.h>

struct state {
  smacq_environment * env;
  int demux;
  struct smacq_outputq * outputq;
  struct batch * batch;
  int num_batches;
  int ipaddr_type;
};

struct batch {
  patricia_tree_t * trie;
  dts_field field;
  char * fieldname;
};

static struct smacq_options options[] = {
  {"f", {string_t:NULL}, "Field to inspect (full data is default)", SMACQ_OPT_TYPE_STRING},
  {"m", {boolean_t:0}, "OR multiple fields and demux to individual outputs", SMACQ_OPT_TYPE_BOOLEAN},
  {NULL, {NULL}, NULL, 0}
};

struct batch * get_batch(struct state * state, char * field) {
	struct batch * batch;
	int i;
	dts_field f = smacq_requirefield(state->env, field);

	for (i = 0; i < state->num_batches; i++) {
		if (dts_comparefields(f, state->batch[i].field)) {
			return &state->batch[i];
		}
	}
	dts_field_free(f);

	state->num_batches++;
	state->batch = realloc(state->batch, state->num_batches * sizeof(struct batch));
	batch = &state->batch[state->num_batches-1];

 	batch->field = smacq_requirefield(state->env, field);
	batch->fieldname = field;
  	batch->trie = New_Patricia(32);

	return batch;
}

static void add_entry(struct state * state, char * field, char * needle, int output) {
  struct batch * batch = NULL;
  prefix_t * pfx;

  if (!field && !needle) return;

  if (!needle) {
  	needle = field;
	field = NULL;
  }

  if (field) {
	batch = get_batch(state, field);
  }

  pfx = ascii2prefix(AF_INET, needle);
  if (!pfx) {
 	fprintf(stderr, "iplookup: unable to parse mask: %s\n", needle);
  	assert(pfx);
  }
  patricia_node_t * node = patricia_lookup(batch->trie, pfx);
    
  assert(node);
  if (node->data) {
  	assert(!"duplicate mask");
  	//deref_data(node->data);
  }
  node->data = (void*)output;
}

static smacq_result iplookup_produce(struct state* state, const dts_object ** datum, int * outchan) {
  return smacq_produce_dequeue(&state->outputq, datum, outchan);
}

static smacq_result iplookup_consume(struct state * state, const dts_object * datum, int * outchan) {
  const dts_object * field;
  int matched = 0;
  int chan;
  int i;
  patricia_node_t * node;

  assert(datum);

  for (i=0; i < state->num_batches; i++) {
     struct batch * batch = &state->batch[i];
     prefix_t * prefix;
     

     if (batch->field) {
  	field = smacq_getfield(state->env, datum, batch->field, NULL);
  	if (!field) return SMACQ_FREE;
     } else {
        field = datum;
     }

     assert(field->type == state->ipaddr_type);
     prefix = New_Prefix(AF_INET, dts_getdata(field), 0);
 
     node = patricia_search_best(batch->trie, prefix);
     Deref_Prefix(prefix);	 
     if (node) {
	  	chan = (int)node->data;
		if (matched) {
			smacq_produce_enqueue(&state->outputq, datum, chan);
			dts_incref(datum, 1);
	  	} else {
	  		matched = 1;
			*outchan = chan;
		} 
     }

     if (batch->field) 
	  dts_decref(field);

  }

  if (matched) {
	return SMACQ_PASS|smacq_produce_canproduce(&state->outputq);
  } else {
  	return SMACQ_FREE;
  }

}

static smacq_result iplookup_init(struct smacq_init * context) {
  struct state * state;
  int i, argc;
  char ** argv;
  smacq_opt field_opt, demux;
  char * field = NULL;
  char * pattern = NULL;
  int output = 0;

  context->state = state = (struct state*) calloc(sizeof(struct state),1);
  assert(state);

  state->batch = calloc(sizeof(struct batch), 1);

  state->env = context->env;
  {
    struct smacq_optval optvals[] = {
      {"f", &field_opt},
      {"m", &demux},
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &argc, &argv,
				 options, optvals);

    state->demux = demux.boolean_t;
  }

  state->ipaddr_type = smacq_requiretype(state->env, "ip");

  for (i = 0; i < argc; i++) {
	  if (!strcmp(argv[i], ";") && pattern) {
		if (!field) field = field_opt.string_t; 
		//fprintf(stderr, "middle pattern is %s\n", pattern);
		add_entry(state, field, pattern, output++);
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
  	add_entry(state, field, pattern, output++);
  }

  if (output>1) {
	  state->demux = 1;
  }

  return 0;
}

static smacq_result iplookup_shutdown(struct state * state) {
  int i;

  for (i=0; i < state->num_batches; i++) {
  	Destroy_Patricia(state->batch[i].trie, NULL);
  }

  free(state);
  return 0;
}

struct smacq_functions smacq_iplookup_table = {
	produce: &iplookup_produce, 
	consume: &iplookup_consume,
	init: &iplookup_init,
	shutdown: &iplookup_shutdown,
	algebra: {vector: 1, boolean: 1}
};

