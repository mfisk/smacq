#include <stdlib.h>
#include <assert.h>
#include <smacq.h>

/* Programming constants */

static struct smacq_options options[] = {
  {NULL, {string_t:NULL}, NULL, 0}
};

struct obj_list{
  dts_object * obj;
  struct obj_list * next;
};

struct state {
  smacq_environment * env;
  struct obj_list * fifo, * last;
}; 

static smacq_result fifo_consume(struct state * state, const dts_object * datum, int * outchan) {
  struct obj_list * newo = g_new(struct obj_list, 1);
  newo->obj = (dts_object*)datum;
  newo->next = NULL;

  if (state->fifo) {
	  state->last->next = newo;
  } else {
	  state->fifo = newo;
  }
  state->last = newo;
  dts_incref(datum, 1);
  return(SMACQ_FREE|SMACQ_CANPRODUCE);
}

static smacq_result fifo_init(struct smacq_init * context) {
  int argc = 0;
  char ** argv;
  struct state * state = context->state = g_new0(struct state, 1);
  state->env = context->env;

  {
  	struct smacq_optval optvals[] = {
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);
  }

  return 0;
}

static smacq_result fifo_produce(struct state * state, const dts_object ** datum, int * outchan) {
  if (state->fifo) {
    struct obj_list * old = state->fifo;
    state->fifo = state->fifo->next;

    *datum = old->obj;
    free(old);
  } else {
    return(SMACQ_END);
  }
  return(SMACQ_PASS|(state->fifo ? SMACQ_CANPRODUCE : 0));
}

struct smacq_functions smacq_fifo_table = {
  produce: &fifo_produce, 
  consume: &fifo_consume,
  init: &fifo_init,
};
