#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
#include <smacq-internal.h>
#include <fields.h>
#include <bytehash.h>

/* Programming constants */

#define KEYBYTES 128

enum mode { ROUND_ROBIN, UNIQUE, BUCKET };

static struct smacq_options options[] = {
  {NULL, {string_t:NULL}, NULL, 0}
};

struct state {
  smacq_environment * env;
  const dts_object * product;
  void * runq;
  struct filter * graph;
  int done; 
  int isfirst;
}; 

static smacq_result sync_consume(struct state * state, const dts_object * datum, int * outchan) {
 int more = flow_sched_iterative(state->graph, datum, &state->product, 
			     &state->runq, state->isfirst);

 more = (more ? 0 : SMACQ_END);

  if (datum && (state->product == datum)) {
	  return SMACQ_PASS|more;
  } else if (state->product) { 
	  state->done = more;
	  return SMACQ_FREE|SMACQ_PRODUCE;
  } else {
	  return SMACQ_FREE|more;
  }
  
  return SMACQ_PASS;
}

static int sync_init(struct smacq_init * context) {
  int argc = 0;
  int i;
  char ** argv;
  struct state * state = context->state = g_new0(struct state, 1);
  state->env = context->env;
  state->isfirst = context->isfirst;

  {
  	struct smacq_optval optvals[] = {
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

  }

  for (i=0; i<argc; i++) {
	  if (!strcmp(argv[i], "\\|")) {
		  argv[i] = "|";
	  }
  }

  state->graph = smacq_build_pipeline(argc, argv);
  flow_start(state->graph, ITERATIVE, state->env->types);
  
  return 0;
}

static int sync_shutdown(struct state * state) {
  return SMACQ_END;
}


static smacq_result sync_produce(struct state * state, const dts_object ** datump, int * outchan) {
  if (state->isfirst && !state->product) {
	sync_consume(state, NULL, outchan);
  }
  if (state->product) {
	*datump = state->product;
	state->product = NULL;
	if (state->isfirst) {
		return SMACQ_PASS| (state->done ? SMACQ_END : SMACQ_PRODUCE);
	} else {
		return SMACQ_PASS| (state->done ? SMACQ_END : 0);
	}
  } else {
	return SMACQ_FREE;
  }
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_sync_table = {
  &sync_produce, 
  &sync_consume,
  &sync_init,
  &sync_shutdown
};
