#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
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
  smacq_graph * self;
  int queryargc;
  char ** queryargv;
  const dts_object * product;

  GHashTableofBytes * hashtable;

  int refresh_type;

  struct output * cont;

  int lastcall;
}; 

struct output {
  smacq_graph * graph;
  struct runq * runq;
  int shutdown:1;
};

/* Bucket may no longer be valid after this call */
static inline int run(struct state * state, struct output* bucket) {
    int more;
    more = smacq_sched_iterative_busy(bucket->graph, &state->product, bucket->runq, 0);
    if (more & SMACQ_END) {
      assert(!bucket->runq);
      smacq_destroy_graph(bucket->graph);
      bucket->graph = NULL;
    }
    if (more) {
      assert(!state->cont);
      state->cont = bucket;
    }

    return more;
}

static smacq_result groupby_consume(struct state * state, const dts_object * datum, int * outchan) {
  struct iovec * partitionv = fields2vec(state->env, datum, &state->fieldset);
  struct output * bucket;
  int status = SMACQ_FREE;
  int more;
  assert(partitionv);

  bucket = bytes_hash_table_lookupv(state->hashtable, partitionv, state->fieldset.num);

  if (bucket && !bucket->graph) {
    int res = bytes_hash_table_removev(state->hashtable, partitionv, state->fieldset.num);
    assert(res);
    /* run() already ended this graph, but we didn't know about it before. */ 
    free(bucket);
    bucket = NULL;
  }

  if (!bucket) {
    bucket =  g_new0(struct output, 1);
    //fprintf(stderr, "New partition %p\n", bucket);
    bucket->graph = smacq_build_pipeline(state->queryargc, state->queryargv);
    smacq_start(bucket->graph, ITERATIVE, state->env->types);
    smacq_sched_iterative_init(bucket->graph, &bucket->runq, 0);
    bytes_hash_table_insertv(state->hashtable, partitionv, state->fieldset.num, bucket);
  } 

  if (dts_gettype(datum) == state->refresh_type) {
    // Handle refresh (kill-off child)
    int res = bytes_hash_table_removev(state->hashtable, partitionv, state->fieldset.num);
    assert(res);
    //fprintf(stderr, "groupby remove from %p\n", state->hashtable);
    smacq_sched_iterative_shutdown(bucket->graph, bucket->runq);
  } else {
    smacq_sched_iterative_input(bucket->graph, datum, bucket->runq);
  }

  /* Run may invalidate the bucket, so we shouldn't use that variable
   * later in this function. */
  run(state, bucket);
  
  // Producing input is just a PASS
  if (state->product == datum) {
    status = SMACQ_PASS;
    state->product = NULL;
    // XXX: memory leak?  need to decref product?
  }

  if (state->product || state->cont) status |= SMACQ_PRODUCE;

  return status;
}

static smacq_result groupby_init(struct smacq_init * context) {
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
  	smacq_getoptsbyname(argc, argv,
			       &argc, &argv,
			       options, optvals);

  }

  fields_init(state->env, &state->fieldset, argc, argv);
  state->hashtable = bytes_hash_table_new(KEYBYTES, CHAIN, NOFREE);

  state->refresh_type = smacq_requiretype(state->env, "refresh");

  return 0;
}

static int destroy_bucket(gpointer key, gpointer value, gpointer userdata) {
  struct state * state = (struct state *)userdata;
  struct output * bucket = (struct output*)value;

  if (state->product) {
	  state->lastcall = 1;

	  /* We can't produce now, but the result of this algorithm
	   * is that foreach() has to be called once for each
	   * bucket that has something to produce.  So if n buckets
	   * have output to produce, foreach is called n times 
	   * and destroy_bucket is called n*(n-1)/2 times. 
	   * We'd rather tell foreach() to stop completely. */
	  return 0;
  }

  if (!bucket->shutdown) {
	bucket->shutdown = 1;
  	//fprintf(stderr, "doing shutdown of %p\n", bucket->graph);
  	smacq_sched_iterative_shutdown(bucket->graph, bucket->runq);
  }

  if ((SMACQ_END & run(state, bucket))) {
  	//fprintf(stderr, "Last call for bucket %p.  Product now %p\n", bucket, state->product);
	return 1;
  } else {
  	//fprintf(stderr, "Postponed last call for bucket %p.  Product now %p\n", bucket, state->product);
	return 0;
  }
}

static smacq_result groupby_produce(struct state * state, const dts_object ** datump, int * outchan) {
  int status = SMACQ_FREE;
  int lastcall = 1;
  *datump = NULL;

  if (state->cont) {
    struct output * cont = state->cont;
    state->cont = NULL;
    run(state, cont);

    lastcall = 0;
  } 
  
  if (state->product) {
    *datump = state->product;
    state->product = NULL;
    status = SMACQ_PASS;

    lastcall = 0;
  }
  
  if (state->lastcall || lastcall) {
    /* This must be last-call, because we didn't ask for it */
    /* Notify all children */
    bytes_hash_table_foreach_remove(state->hashtable, destroy_bucket, state);
    //fprintf(stderr, "finished a round of last call with %p, %p, %p\n", *datump, state->product, state->cont);
  }

  if (!*datump && (state->cont || state->product)) {
	return groupby_produce(state, datump, outchan);
  }

  return status | ((state->cont || state->product) ? SMACQ_PRODUCE : 0); 
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_groupby_table = {
	produce: &groupby_produce, 
	consume: &groupby_consume,
	init: &groupby_init,
	shutdown: NULL,
	algebra: { demux: 1 }
};
