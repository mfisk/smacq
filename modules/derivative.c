#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
#include <dts_packet.h>
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
  dts_field derivfield, doubletransform;
}; 
 
static smacq_result derivative_consume(struct state * state, const dts_object * datum, int * outchan) {
  dts_object newx, newy;
  dts_object newxp, newyp;

  if (!smacq_getfield(state->env, datum, state->xfield, &newx)) {
	fprintf(stderr, "derivative: no %s field\n", state->xfieldname);
	return SMACQ_PASS;
  }
  if (!smacq_getfield(state->env, datum, state->yfield, &newy)) {
	fprintf(stderr, "derivative: no %s field\n", state->yfieldname);
	return SMACQ_PASS;
  }

  if (1 > smacq_getfield(state->env, &newx, state->doubletransform, &newxp)) {
	fprintf(stderr, "derivative: can't convert field %s to double\n", state->xfieldname);
	return SMACQ_PASS;
  }
  if (1 > smacq_getfield(state->env, &newy, state->doubletransform, &newyp)) {
	fprintf(stderr, "derivative: can't convert field %s to double\n", state->yfieldname);
	//free(*newxp);
	return SMACQ_PASS;
  }

  // assert(newx.type == state->doubletype);
  // assert(newy.type == state->doubletype);

  if (state->started) {
	double dydx = (dts_data_as(&newyp, double) - state->lasty) / 
			(dts_data_as(&newxp, double) - state->lastx);
    	dts_object * msgdata = smacq_dts_construct(state->env, state->derivtype, &dydx);
    	dts_attach_field(datum, state->derivfield, msgdata); 
	//fprintf(stderr, "%g - %g / %g - %g\n", *newyp, state->lasty, *newxp, state->lastx);
  } else {
	state->started = 1;
  }

  //free(*newxp);
  //free(*newyp);

  state->lastx = dts_data_as(&newxp, double);
  state->lasty = dts_data_as(&newyp, double);
	
  return SMACQ_PASS;
}

static int derivative_init(struct smacq_init * context) {
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
  
  state->xfieldname = argv[1];
  state->yfieldname = argv[0];

  state->xfield = smacq_requirefield(state->env, state->xfieldname);
  state->yfield = smacq_requirefield(state->env, state->yfieldname);

  state->doubletransform = smacq_requirefield(state->env, "double");

  return 0;
}

static int derivative_shutdown(struct state * state) {
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
