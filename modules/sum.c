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
  {"r", {boolean_t:0}, "Output sum only on refresh", SMACQ_OPT_TYPE_BOOLEAN},
  {NULL, {string_t:NULL}, NULL, 0}
};

struct state {
  smacq_environment * env;
  struct fieldset fieldset;
  GHashTableofBytes *counters;

  double total;
  dts_field xfield;
  char * xfieldname;

  int sumtype;
  dts_field sumfield;
  int refreshtype;
  
  int refreshonly;
}; 
 
static smacq_result sum_consume(struct state * state, const dts_object * datum, int * outchan) {
  const dts_object * newx;
  dts_object * msgdata;
  
  if (dts_gettype(datum) != state->refreshtype) {
    if (! (newx = smacq_getfield(state->env, datum, state->xfield, NULL))) {
      fprintf(stderr, "sum: no %s field\n", state->xfieldname);
      return SMACQ_PASS;
    }
    
    // assert(newx.type == state->doubletype);

    state->total += dts_data_as(newx, double);
    dts_decref(newx);
  }

  if ( (!state->refreshonly) || (dts_gettype(datum) == state->refreshtype)) {
    msgdata = smacq_dts_construct(state->env, state->sumtype, &state->total);
    dts_attach_field(datum, state->sumfield, msgdata); 
	
    return SMACQ_PASS;
  } else {
    return SMACQ_FREE;
  }
    
}

static int sum_init(struct smacq_init * context) {
  int argc = 0;
  char ** argv;
  struct state * state = context->state = g_new0(struct state, 1);
  state->env = context->env;


  {
    smacq_opt refresh;

    struct smacq_optval optvals[] = {
      {"r", &refresh},
      {NULL, NULL}
    };

    smacq_getoptsbyname(context->argc-1, context->argv+1,
		       &argc, &argv,
		       options, optvals);

    state->refreshonly = refresh.boolean_t;
  }

  assert(argc==1);

  state->refreshtype = smacq_requiretype(state->env, "refresh");
  state->sumtype = smacq_requiretype(state->env, "double");
  state->sumfield = smacq_requirefield(state->env, "sum");
  
  state->xfieldname = dts_fieldname_append(argv[0], "double");
  state->xfield = smacq_requirefield(state->env, state->xfieldname);

  return 0;
}

static int sum_shutdown(struct state * state) {
  free(state);
  return 0;
}


static smacq_result sum_produce(struct state * state, const dts_object ** datum, int * outchan) {
  return SMACQ_END;
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_sum_table = {
  &sum_produce, 
  &sum_consume,
  &sum_init,
  &sum_shutdown
};
