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
  {NULL, {string_t:NULL}, NULL, 0}
};

struct state {
  smacq_environment * env;
  struct fieldset fieldset;
  GHashTableofBytes *counters;
  int started;

  double lastx, lasty;
  dts_field xfield, yfield;
  char * xfieldname, * yfieldname;

  int derivtype;
  dts_field derivfield;
}; 
 
static smacq_result derivative_consume(struct state * state, const dts_object * datum, int * outchan) {
  const dts_object * newx, * newy;

  if (! (newx = smacq_getfield(state->env, datum, state->xfield, NULL))) {
	fprintf(stderr, "derivative: no %s field\n", state->xfieldname);
	return SMACQ_PASS;
  }
  if (! (newy = smacq_getfield(state->env, datum, state->yfield, NULL))) {
	fprintf(stderr, "derivative: no %s field\n", state->yfieldname);
	dts_decref(newx);
	return SMACQ_PASS;
  }

  if (state->started) {
	double dydx = (dts_data_as(newy, double) - state->lasty) / 
			(dts_data_as(newx, double) - state->lastx);
    	dts_object * msgdata = smacq_dts_construct(state->env, state->derivtype, &dydx);
    	dts_attach_field(datum, state->derivfield, msgdata); 
  } else {
	state->started = 1;
  }

  state->lastx = dts_data_as(newx, double);
  state->lasty = dts_data_as(newy, double);
	
  dts_decref(newx);
  dts_decref(newy);

  return SMACQ_PASS; 
}

static smacq_result derivative_init(struct smacq_init * context) {
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

  assert(argc==2);

  state->derivtype = smacq_requiretype(state->env, "double");
  state->derivfield = smacq_requirefield(state->env, "derivative");

  state->yfieldname = dts_fieldname_append(argv[0], "double");
  state->xfieldname = dts_fieldname_append(argv[1], "double");

  state->xfield = smacq_requirefield(state->env, state->xfieldname);
  state->yfield = smacq_requirefield(state->env, state->yfieldname);

  return 0;
}

static smacq_result derivative_shutdown(struct state * state) {
  return 0;
}


static smacq_result derivative_produce(struct state * state, const dts_object ** datum, int * outchan) {
  return SMACQ_END;
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_derivative_table = {
  &derivative_produce, 
  &derivative_consume,
  &derivative_init,
  &derivative_shutdown
};
