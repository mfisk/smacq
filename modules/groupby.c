#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
#include <flow-internal.h>
#include <fields.h>
#include <bytehash.h>

/* Programming constants */

#define KEYBYTES 128

static struct smacq_options options[] = {
  {NULL, {string_t:NULL}, NULL, 0}
};

struct state {
  smacq_environment * env;
  struct fieldset fieldset;
  struct filter * self;
  int queryargc;
  char ** queryargv;
  const dts_object * product;

  GHashTableofBytes * hashtable;

  int refresh_type;

  struct output * cont;
}; 

struct output {
  struct filter * graph;
  void * runq;
};

static smacq_result groupby_consume(struct state * state, const dts_object * datum, int * outchan) {
  struct iovec * partitionv = fields2vec(state->env, datum, &state->fieldset);
  struct output * bucket;
  int status = SMACQ_FREE;
  int more;
  assert(partitionv);

  bucket = bytes_hash_table_lookupv(state->hashtable, partitionv, state->fieldset.num);

  if (!bucket) {
    bucket =  g_new(struct output, 1);
    bucket->runq = NULL;
    //fprintf(stderr, "Cloning %d\n", state->bucket);
    bucket->graph = smacq_build_pipeline(state->queryargc, state->queryargv);
    //bucket = smacq_add_child(state->self, bucket->graph);
    flow_start(bucket->graph, ITERATIVE, state->env->types);
    bytes_hash_table_insertv(state->hashtable, partitionv, state->fieldset.num, bucket);
  } 

  more = flow_sched_iterative(bucket->graph, datum, &state->product, &bucket->runq, 0);
  
  // Producing input is just a PASS
  if (state->product == datum) {
    status = SMACQ_PASS;

    if (! (more&SMACQ_END)) {
      more = flow_sched_iterative(bucket->graph, NULL, &state->product, &bucket->runq, 0);
    } else {
      state->product = NULL;
    }
  }
  
  // Handle refresh (kill-off child)
  if (dts_gettype(datum) == state->refresh_type) {
    bytes_hash_table_removev(state->hashtable, partitionv, state->fieldset.num);
    flow_sched_iterative_shutdown(bucket->graph, &bucket->runq);
    
    if (!state->product) {
      // Null datum is EOF
      more = flow_sched_iterative(bucket->graph, NULL, &state->product, &bucket->runq, 0);
    }
  }
  
  if (more & SMACQ_END) {
    free(bucket);
  } else {
    state->cont = bucket;
  }

  if (state->product) status |= SMACQ_PRODUCE;

  return status;
}

static int groupby_init(struct flow_init * context) {
  int argc = 0;
  char ** argv;
  int i;
  struct state * state = context->state = g_new0(struct state, 1);
  state->env = context->env;
  state->self = context->self;
  argc=context->argc-1;
  argv=context->argv+1;

  for (i=0; i<argc; i++) {
    if (!strcmp(argv[i], "\\|")) {
      argv[i] = "|";
    }
  }

  state->queryargc = 0;
  for (i=0; i<argc; i++) {
    if (!strcmp(argv[i], "--")) {
      state->queryargc = argc - i - 1;
      state->queryargv = argv+(i+1);
      break;
    }
  }

  assert(state->queryargc);
  argc -= (state->queryargc +1);

  // Consume rest of arguments as fieldnames
  assert(argc > 0);

  {
  	struct smacq_optval optvals[] = {
    		{NULL, NULL}
  	};
  	flow_getoptsbyname(argc, argv,
			       &argc, &argv,
			       options, optvals);

  }

  fields_init(state->env, &state->fieldset, argc, argv);
  state->hashtable = bytes_hash_table_new(KEYBYTES, CHAIN, NOFREE);

  state->refresh_type = flow_requiretype(state->env, "refresh");

  return 0;
}

static int groupby_shutdown(struct state * state) {
  return SMACQ_END;
}


static smacq_result groupby_produce(struct state * state, const dts_object ** datump, int * outchan) {
  if (state->product) {
    *datump = state->product;

    if (state->cont) {
      if (!flow_sched_iterative(state->cont->graph, NULL, &state->product, &state->cont->runq, 0)) {
	state->cont = NULL;
      }
    } else {
      assert(0);
      state->product = NULL;
    }

    return SMACQ_PASS | (state->product ? SMACQ_PRODUCE : 0);
  } else {
    assert(0);
    return SMACQ_FREE;
  }
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_groupby_table = {
  &groupby_produce, 
  &groupby_consume,
  &groupby_init,
  &groupby_shutdown
};
