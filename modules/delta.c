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

  double lastx;
  int xfield;
  char * xfieldname;

  int deltatype, deltafield;
}; 
 
static smacq_result delta_consume(struct state * state, const dts_object * datum, int * outchan) {
  dts_object newx;
  double * newxp;
  int newxpsize;

  if (!smacq_getfield(state->env, datum, state->xfield, &newx)) {
	fprintf(stderr, "delta: no %s field\n", state->xfieldname);
	return SMACQ_PASS;
  }

  if (1 > smacq_presentdata(state->env, &newx, smacq_transform(state->env, "double"), (void*)&newxp, &newxpsize)) {
	fprintf(stderr, "delta: can't convert field %s to double\n", state->xfieldname);
	return SMACQ_PASS;
  }

  // assert(newx.type == state->doubletype);

  if (state->started) {
	double delta = *newxp - state->lastx;
    	dts_object * msgdata = smacq_dts_construct(state->env, state->deltatype, &delta);
    	dts_attach_field(datum, state->deltafield, msgdata); 
  } else {
	state->started = 1;
  }

  state->lastx = *newxp;
  free(newxp);
	
  return SMACQ_PASS;
}

static int delta_init(struct smacq_init * context) {
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

  assert(argc==1);

  state->deltatype = smacq_requiretype(state->env, "double");
  state->deltafield = smacq_requirefield(state->env, "delta");
  
  state->xfieldname = argv[0];
  state->xfield = smacq_requirefield(state->env, state->xfieldname);

  return 0;
}

static int delta_shutdown(struct state * state) {
  return 0;
}


static smacq_result delta_produce(struct state * state, const dts_object ** datum, int * outchan) {
  return SMACQ_END;
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_delta_table = {
  &delta_produce, 
  &delta_consume,
  &delta_init,
  &delta_shutdown
};
