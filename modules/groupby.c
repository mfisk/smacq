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
  int done;

  GHashTableofBytes * hashtable;

  int refresh_type;
}; 

struct output {
  struct filter * graph;
  void * runq;
};

static smacq_result groupby_consume(struct state * state, const dts_object * datum, int * outchan) {
  struct iovec * partitionv = fields2vec(state->env, datum, &state->fieldset);
  struct output * bucket;
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

  {
    int more = flow_sched_iterative(bucket->graph, datum, &state->product, &bucket->runq, 0);

    more = (more ? 0 : SMACQ_END);
    
    if (datum && (state->product == datum)) {
      return SMACQ_PASS|more;
    } else if (state->product) { 
      state->done = more;
      return SMACQ_FREE|SMACQ_PRODUCE;
    } else {
      return SMACQ_FREE|more;
    }
  }

  return SMACQ_PASS;
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
  state->hashtable = bytes_hash_table_new(KEYBYTES, chain);

  state->refresh_type = flow_requiretype(state->env, "refresh");

  return 0;
}

static int groupby_shutdown(struct state * state) {
  return SMACQ_END;
}


static smacq_result groupby_produce(struct state * state, const dts_object ** datump, int * outchan) {
  if (state->product) {
    *datump = state->product;
    state->product = NULL;
    return SMACQ_PASS| (state->done ? SMACQ_END : 0);
  } else {
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
