#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
#include <fields.h>
#include <bytehash.h>

/* Programming constants */

#define KEYBYTES 128

enum mode { ROUND_ROBIN, UNIQUE, BUCKET };

static struct smacq_options options[] = {
  {"b", {int_t:0}, "Number of ways to split", SMACQ_OPT_TYPE_INT},
  {NULL, {string_t:NULL}, NULL, 0}
};

SMACQ_MODULE(split,
  PROTO_CTOR(split);
  PROTO_CONSUME();

  struct fieldset fieldset;
  int children;
  int bucket;
  enum mode mode;
  smacq_graph * self;

  struct iovec_hash * hashtable;
); 

smacq_result splitModule::consume(DtsObject * datum, int * outchan) {
  int bucket;
  struct iovec * partitionv = datum->fields2vec(&fieldset);
	
  if (mode == ROUND_ROBIN) {
    // Round-robin
    bucket = bucket;
    bucket = ((bucket+1)  % children);
  } else if (!partitionv) {
    bucket = 0;
  } else if (mode == BUCKET) {
    bucket = bytes_hashv_into(partitionv, fieldset.num, children);
  } else if (mode == UNIQUE) {
    bucket = (int)bytes_hash_table_lookupv(hashtable, partitionv, fieldset.num);
    if (!bucket) {
	smacq_graph * newClone;
	bucket = bucket++;
	//fprintf(stderr, "Cloning %d\n", bucket);
        newClone = smacq_clone_tree(self, self, 0);
	smacq_init_modules(newClone, dts);

	// 0 return value is error, so everything is inflated by 1
    	bytes_hash_table_setv(hashtable, partitionv, fieldset.num, (gpointer)bucket);
    } else { 
	// 0 return value is error, so everything is inflated by 1
	bucket--;
    }
  } else {
    assert(0);
  }

  // bucket = (bucket++) % children;
  //fprintf(stderr, "Sending to output #%d\n", bucket);
  *outchan = bucket;
  return SMACQ_PASS;
}

splitModule::splitModule(struct smacq_init * context) : SmacqModule(context) {
  int argc = 0;
  char ** argv;
  int i;
  self = context->self;

  {
	smacq_opt buckets;

  	struct smacq_optval optvals[] = {
    		{"b", &buckets},
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

	children = buckets.int_t;
  }

  // Consume rest of arguments as fieldnames
  if (argc) {
  	if (children) {
		fprintf(stderr, "Cannot specify both -b and fields\n");
		assert(0);
	}

  	mode = UNIQUE;
  	dts->fields_init(&fieldset, argc, argv);
  	hashtable = bytes_hash_table_new(KEYBYTES, CHAIN|NOFREE);
  } else if (!children) {
	fprintf(stderr, "Must specify either -b or fields\n");
	assert(0);
  } else {
	mode = ROUND_ROBIN;

  	for  (i=1; i < children; i++) {
    		smacq_clone_tree(context->self, context->self, 0);
  	}
  }
}

