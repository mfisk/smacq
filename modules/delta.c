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
  struct iovec_hash *counters;
  int started;

  double lastx;
  dts_field xfield;
  char * xfieldname;

  int deltatype;
  dts_field deltafield;
}; 
 
static smacq_result delta_consume(struct state * state, const dts_object * datum, int * outchan) {
  const dts_object * newx;

  if (! (newx = smacq_getfield(state->env, datum, state->xfield, NULL))) {
	fprintf(stderr, "delta: no %s field\n", state->xfieldname);
	return SMACQ_PASS;
  }

  // assert(newx->type == state->doubletype);

  if (state->started) {
	double delta = dts_data_as(newx, double) - state->lastx;
    	dts_object * msgdata = smacq_dts_construct(state->env, state->deltatype, &delta);
    	dts_attach_field(datum, state->deltafield, msgdata); 
  } else {
	state->started = 1;
  }

  state->lastx = dts_data_as(newx, double);
  dts_decref(newx);
	
  return SMACQ_PASS;
}

static smacq_result delta_init(struct smacq_init * context) {
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
  
  state->xfieldname = dts_fieldname_append(argv[0], "double");
  state->xfield = smacq_requirefield(state->env, state->xfieldname);

  return 0;
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_delta_table = {
  consume: &delta_consume,
  init: &delta_init,
  algebra: { nesting: 1}
};
