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

struct state {
  smacq_environment * env;
  struct fieldset fieldset;
  int children;
  int bucket;
  enum mode mode;
  struct filter * self;

  GHashTableofBytes * hashtable;
}; 

static smacq_result split_consume(struct state * state, const dts_object * datum, int * outchan) {
  int bucket;
  struct iovec * partitionv = fields2vec(state->env, datum, &state->fieldset);
	
  if (state->mode == ROUND_ROBIN) {
    // Round-robin
    bucket = state->bucket;
    state->bucket = ((state->bucket+1)  % state->children);
  } else if (!partitionv) {
    bucket = 0;
  } else if (state->mode == BUCKET) {
    bucket = (bytes_hash_valuev(state->hashtable, state->fieldset.num, partitionv) / 2  % state->children);
  } else if (state->mode == UNIQUE) {
    bucket = (int)bytes_hash_table_lookupv(state->hashtable, partitionv, state->fieldset.num);
    if (!bucket) {
	struct filter * newClone;
	bucket = state->bucket++;
	//fprintf(stderr, "Cloning %d\n", state->bucket);
        newClone = smacq_clone_tree(state->self, state->self, 0);
	flow_init_modules(newClone, state->env);

	// 0 return value is error, so everything is inflated by 1
    	bytes_hash_table_insertv(state->hashtable, partitionv, state->fieldset.num, (gpointer)state->bucket);
    } else { 
	// 0 return value is error, so everything is inflated by 1
	bucket--;
    }
  }

  // bucket = (state->bucket++) % state->children;
  //fprintf(stderr, "Sending to output #%d\n", bucket);
  *outchan = bucket;
  return SMACQ_PASS;
}

static int split_init(struct smacq_init * context) {
  int argc = 0;
  char ** argv;
  int i;
  struct state * state = context->state = g_new0(struct state, 1);
  state->env = context->env;
  state->self = context->self;

  {
	smacq_opt buckets;

  	struct smacq_optval optvals[] = {
    		{"b", &buckets},
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

	state->children = buckets.int_t;
  }

  // Consume rest of arguments as fieldnames
  if (argc) {
  	if (state->children) {
		fprintf(stderr, "Cannot specify both -b and fields\n");
		assert(0);
	}

  	state->mode = UNIQUE;
  	fields_init(state->env, &state->fieldset, argc, argv);
  	state->hashtable = bytes_hash_table_new(KEYBYTES, CHAIN, NOFREE);
  } else if (!state->children) {
	fprintf(stderr, "Must specify either -b or fields\n");
	assert(0);
  } else {
	state->mode = ROUND_ROBIN;

  	for  (i=1; i < state->children; i++) {
    		smacq_clone_tree(context->self, context->self, 0);
  	}
  }

  return 0;
}

static int split_shutdown(struct state * state) {
  return SMACQ_END;
}


static smacq_result split_produce(struct state * state, const dts_object ** datump, int * outchan) {
  return SMACQ_END;
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_split_table = {
  &split_produce, 
  &split_consume,
  &split_init,
  &split_shutdown
};
