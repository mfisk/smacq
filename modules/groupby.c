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
  smacq_graph * mastergraph;
  int queryargc;
  char ** queryargv;
  struct smacq_outputq * outputq;

  struct iovec_hash * hashtable;

  int refresh_type;

  struct output * cont;

  struct iovec * partitionv;
}; 

struct output {
  smacq_graph * graph;
  struct runq * runq;
};

/* Bucket may no longer be valid after this call */
static inline int run_and_queue(struct state * state, struct output* partition) {
    int more;
    do {
	const dts_object * product = NULL;
    	more = smacq_sched_iterative_busy(partition->graph, &product, partition->runq, 0);
	/* fprintf(stderr, "run_and_queue produced %p with status %x\n", product, more); */
	if (product) smacq_produce_enqueue(&state->outputq, product, -1);
    	if (more & SMACQ_END) {
      		assert(!partition->runq);
      		smacq_destroy_graph(partition->graph);
      		partition->graph = NULL;
		break;
    	}
    } while (more);
    return more;
}

static inline void destroy_partition(struct state * state, struct output * partition) {
  //fprintf(stderr, "doing shutdown of %p\n", partition->graph);
  smacq_sched_iterative_shutdown(partition->graph, partition->runq);
  run_and_queue(state, partition);
}

static int destroy_partition_wrapper(struct element * key, void * value, void * userdata) {
  struct state * state = (struct state *)userdata;
  struct output * partition = (struct output*)value;

  destroy_partition(state, partition);

  return 1;
}


/* Bucket may no longer be valid after this call */
static inline int run(struct state * state, struct output* partition) {
    int more;
    const dts_object * product = NULL;
    more = smacq_sched_iterative_busy(partition->graph, &product, partition->runq, 0);
    if (product) smacq_produce_enqueue(&state->outputq, product, -1);
    if (more & SMACQ_END) {
      assert(!partition->runq);
      smacq_destroy_graph(partition->graph);
      partition->graph = NULL;
    }
    if (more) {
      assert(!state->cont);
      state->cont = partition;
    }

    return more;
}

static inline struct output * get_partition(struct state * state, struct iovec * partitionv) {
  struct output * partition;

  partition = bytes_hash_table_lookupv(state->hashtable, state->partitionv, state->fieldset.num);

  if (partition && !partition->graph) {
    int res = bytes_hash_table_removev(state->hashtable, state->partitionv, state->fieldset.num);
    assert(res);
    /* run() already ended this graph, but we didn't know about it before. */ 
    free(partition);
    partition = NULL;
  }

  if (!partition) {
    partition =  g_new0(struct output, 1);
    partition->graph = smacq_graph_clone(state->env, state->mastergraph);
    /*
    fprintf(stderr, "New partition %p (graph = %p) running %s %s (%d)\n", partition, partition->graph, state->queryargv[0], state->queryargv[1], state->queryargc);

    fprintf(stderr,"New partition:\n");
    smacq_graph_print(stderr, partition->graph, 1);
    fprintf(stderr,":\n");
    */

    smacq_start(partition->graph, ITERATIVE, state->env->types);
    smacq_sched_iterative_init(partition->graph, &partition->runq, 0);
    bytes_hash_table_setv(state->hashtable, state->partitionv, state->fieldset.num, partition);
  } 

  return partition;
}

static int check_invalidate(struct element * key, void * value, void * userdata) {
  struct state * state = (struct state *)userdata;
  struct element * element = (struct element*)key;
  struct output * partition = (struct output*)value;

  if (bytes_mask(element, state->partitionv, state->fieldset.num)) {
    //fprintf(stderr, "groupby got a partial refresh\n");
    destroy_partition(state, partition);
    return 1;
  }

  return 0;
}

static inline void handle_invalidate(struct state * state, const dts_object * datum) {
   bytes_hash_table_foreach_remove(state->hashtable, check_invalidate, state);
}

static smacq_result groupby_consume(struct state * state, const dts_object * datum, int * outchan) {
  struct output * partition;
  int status = SMACQ_FREE;

  //fprintf(stderr , "groupby got %p a type %d (refresh is %d)\n", datum, dts_gettype(datum), state->refresh_type);

  state->partitionv = fields2vec(state->env, datum, &state->fieldset);

  if (iovec_has_undefined(state->partitionv, state->fieldset.num)) {
  	if (dts_gettype(datum) == state->refresh_type) {
    		handle_invalidate(state, datum);
	} else {
		fprintf(stderr, "Fatal Error: Groupby was passed a data object that did not have the fields it is supposed to group by!\n");
		assert(0);
	}
  } else {
     partition = get_partition(state, state->partitionv);

     if (dts_gettype(datum) == state->refresh_type) {
       int res;
       // Handle refresh (kill-off child)
       destroy_partition(state, partition);
       res = bytes_hash_table_removev(state->hashtable, state->partitionv, state->fieldset.num);
       assert(res);
     } else {
       smacq_sched_iterative_input(partition->graph, datum, partition->runq);
       run(state, partition);
     }
  }
  
  // Producing input is just a PASS
  if (smacq_produce_peek(&state->outputq) == datum) {
    smacq_produce_dequeue(&state->outputq, &datum, outchan);
    status = SMACQ_PASS;
  }

  if (smacq_produce_canproduce(&state->outputq) || state->cont) status |= SMACQ_PRODUCE;

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
      state->queryargv = argv+(i+1);
      state->queryargc = argc-(i+1);
      break;
    }
  }

  if (!state->queryargc) {
	  fprintf(stderr, "Warning: groupby called with no aggregates to execute!\n");
	  assert(0);
  }
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
  state->hashtable = bytes_hash_table_new(KEYBYTES, CHAIN|NOFREE);

  state->refresh_type = smacq_requiretype(state->env, "refresh");

  state->mastergraph = smacq_build_query(state->env->types, state->queryargc, state->queryargv);

  return 0;
}

static smacq_result groupby_produce(struct state * state, const dts_object ** datump, int * outchan) {
  int status;
  int lastcall = 1;
  *datump = NULL;

  if (state->cont) {
    struct output * cont = state->cont;
    state->cont = NULL;
    run(state, cont);

    status = SMACQ_FREE;
    lastcall = 0;
  } 
  
  if (smacq_produce_canproduce(&state->outputq)) {
    status = smacq_produce_dequeue(&state->outputq, datump, outchan);
    lastcall = 0;
  }

  if (lastcall) {
    /* This must be last-call, because we didn't ask for it */
    /* Notify all children */
    /* fprintf(stderr, "groupby got last call\n"); */
    bytes_hash_table_foreach_remove(state->hashtable, destroy_partition_wrapper, state);
    status = smacq_produce_dequeue(&state->outputq, datump, outchan);
  }

  if (state->cont) status |= SMACQ_PRODUCE;

  return (status | smacq_produce_canproduce(&state->outputq)); 
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_groupby_table = {
	produce: &groupby_produce, 
	consume: &groupby_consume,
	init: &groupby_init,
	algebra: { demux: 1 }
};
