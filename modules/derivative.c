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
  int xfield, yfield;
  char * xfieldname, * yfieldname;

  int derivtype, derivfield;
  int doubletransform;
}; 
 
static smacq_result derivative_consume(struct state * state, const dts_object * datum, int * outchan) {
  dts_object newx, newy;
  double * newxp, * newyp;
  int newxpsize, newypsize;

  if (!flow_getfield(state->env, datum, state->xfield, &newx)) {
	fprintf(stderr, "derivative: no %s field\n", state->xfieldname);
	return SMACQ_PASS;
  }
  if (!flow_getfield(state->env, datum, state->yfield, &newy)) {
	fprintf(stderr, "derivative: no %s field\n", state->yfieldname);
	return SMACQ_PASS;
  }

  if (1 > flow_presentdata(state->env, &newx, state->doubletransform, (void*)&newxp, &newxpsize)) {
	fprintf(stderr, "derivative: can't convert field %s to double\n", state->xfieldname);
	return SMACQ_PASS;
  }
  if (1 > flow_presentdata(state->env, &newy, state->doubletransform, (void*)&newyp, &newypsize)) {
	fprintf(stderr, "derivative: can't convert field %s to double\n", state->yfieldname);
	//free(*newxp);
	return SMACQ_PASS;
  }

  // assert(newx.type == state->doubletype);
  // assert(newy.type == state->doubletype);

  if (state->started) {
	double dydx = (*newyp - state->lasty) / (*newxp - state->lastx);
    	dts_object * msgdata = flow_dts_construct(state->env, state->derivtype, &dydx);
    	dts_attach_field(datum, state->derivfield, msgdata); 
	//fprintf(stderr, "%g - %g / %g - %g\n", *newyp, state->lasty, *newxp, state->lastx);
  } else {
	state->started = 1;
  }

  //free(*newxp);
  //free(*newyp);

  state->lastx = *newxp;
  state->lasty = *newyp;
	
  return SMACQ_PASS;
}

static int derivative_init(struct flow_init * context) {
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

  state->derivtype = flow_requiretype(state->env, "double");
  state->derivfield = flow_requirefield(state->env, "derivative");
  
  state->xfieldname = argv[1];
  state->yfieldname = argv[0];

  state->xfield = flow_requirefield(state->env, state->xfieldname);
  state->yfield = flow_requirefield(state->env, state->yfieldname);

  state->doubletransform = flow_transform(state->env, "double");

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
