#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
#include <fields.h>
#include "bytehash.h"

static struct smacq_options options[] = {
  {"t", {double_t:1}, "Clock tick length", SMACQ_OPT_TYPE_DOUBLE},
  {NULL, {string_t:NULL}, NULL, 0}
};

struct state {
  smacq_environment * env;

  dts_field timefield;
  dts_field clockfield;
  int refreshtype;
  int ticktype;
  double period;
  struct smacq_outputq * outputq;

  double current;
  const dts_object * ticko;
}; 
 
static smacq_result clock_consume(struct state * state, const dts_object * datum, int * outchan) {
  const dts_object * ticko; 
  unsigned long tick;

  {
      	const dts_object * time;
	time = smacq_getfield(state->env, datum, state->timefield, NULL);
  	if (!time) return SMACQ_PASS;

  	tick = dts_data_as(time, double);
  	dts_decref(time);
  }
  tick /= state->period;
  //fprintf(stderr, "got time tick %f (period %f)\n", tick, state->period);

  ticko = smacq_dts_construct(state->env, state->ticktype, &tick); 
  assert(ticko);
  dts_attach_field(datum, state->clockfield, ticko);

  if ( tick != state->current) {
	dts_object * refresh = smacq_dts_construct(state->env, state->refreshtype, " ");
	dts_attach_field(refresh, state->clockfield, state->ticko);
  	//fprintf(stderr, "clock is queueing %p (a refresh of type %d)\n", refresh, state->refreshtype);
	smacq_produce_enqueue(&state->outputq, refresh, -1);
	state->current = tick;
	state->ticko = ticko;
	dts_incref(ticko, 1);
  }

  return SMACQ_PASS|smacq_produce_canproduce(&state->outputq);
}

static smacq_result clock_init(struct smacq_init * context) {
  int argc = 0;
  char ** argv;
  struct state * state = context->state = g_new0(struct state, 1);
  state->env = context->env;

  {
    smacq_opt period;

    struct smacq_optval optvals[] = {
      {"t", &period},
      {NULL, NULL}
    };

    smacq_getoptsbyname(context->argc-1, context->argv+1,
		       &argc, &argv,
		       options, optvals);

    state->period = period.double_t;
  }

  assert(argc);
  state->timefield = smacq_requirefield(state->env, dts_fieldname_append(argv[0],"double"));
  state->clockfield = smacq_requirefield(state->env, "clock");
  state->refreshtype = smacq_requiretype(state->env, "refresh");
  state->ticktype = smacq_requiretype(state->env, "int");
  
  return 0;
}

static smacq_result clock_shutdown(struct state * state) {
  free(state);
  return 0;
}

static smacq_result clock_produce(struct state * state, const dts_object ** datum, int * outchan) {
  //fprintf(stderr, "clock is producing (a refresh)\n");
  if (smacq_produce_peek(&state->outputq)) {
  	return smacq_produce_dequeue(&state->outputq, datum, outchan);
  } else {
	/* Forced last call */
	*datum = smacq_dts_construct(state->env, state->refreshtype, NULL);
	dts_attach_field(*datum, state->clockfield, state->ticko);
	return (SMACQ_PASS|SMACQ_END);
  }
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_clock_table = {
  produce: &clock_produce, 
  consume: &clock_consume,
  init: &clock_init,
  shutdown: &clock_shutdown,
  algebra: { nesting: 1},
};
