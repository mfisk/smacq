#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>

/* Programming constants */

#define VECTORSIZE 21
#define ALARM_BITS 15
#define KEYBYTES 128

static struct smacq_options options[] = {
  {NULL, {string_t:NULL}, NULL, 0}
};

struct state {
  smacq_environment * env;

  double total, prev_total;

  dts_field probfield; 
  dts_field entropyfield;
  int refreshtype;
  int probtype;

  const dts_object * lasto;
}; 
 
static smacq_result entropy_consume(struct state * state, const dts_object * datum, int * outchan) {
	if (dts_gettype(datum) == state->refreshtype) {
		double total = state->total / log(2);
    		dts_object * msgdata = smacq_dts_construct(state->env, state->probtype, &total);
		// fprintf(stderr, "Got refresh\n");
    		dts_attach_field(datum, state->entropyfield, msgdata); 
		//dts_incref(msgdata, 1);

		state->prev_total = state->total;
		state->total = 0;
	
		return SMACQ_PASS;
	} else {
		const dts_object * probo;
		double prob;

		// Keep last datum around so we can use it to spit out data when we're ready
		// fprintf(stderr, "replacing cache of %p(%d) with %p(%d)\n", state->lasto, state->lasto ? dts_gettype(state->lasto) : -1, datum, dts_gettype(datum));
		assert(state->lasto != datum);
		if (state->lasto) dts_decref(state->lasto);
		state->lasto = datum;
		dts_incref(datum, 1);

		if (!(probo = smacq_getfield(state->env, datum, state->probfield, NULL))) {
			fprintf(stderr, "No probability field\n");
			return SMACQ_PASS;
		}
		prob = dts_data_as(probo, double);
		dts_decref(probo);
		state->total -= prob * log(prob);
  		
		return SMACQ_FREE;
	}
}

static smacq_result entropy_init(struct smacq_init * context) {
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

  state->refreshtype = smacq_requiretype(state->env, "refresh");
  state->probfield = smacq_requirefield(state->env, dts_fieldname_append("probability", "double"));
  state->probtype = smacq_requiretype(state->env, "double");
  state->entropyfield = smacq_requirefield(state->env, "entropy");

  return 0;
}

static smacq_result entropy_shutdown(struct state * state) {
  return 0;
}


static smacq_result entropy_produce(struct state * state, const dts_object ** datum, int * outchan) {
  double total = state->total;
  if (!state->total) total = state->prev_total;

  total /= log(2);
  // fprintf(stderr, "TOTAL is %g\n", total);

  if (!total) {
    return SMACQ_END;
  } else {
    //dts_object * refresh = smacq_dts_construct(state->env, state->refreshtype, NULL);
    dts_object * msgdata = smacq_dts_construct(state->env, state->probtype, &total);
    dts_attach_field(state->lasto, state->entropyfield, msgdata); 
    //dts_incref(msgdata, 1);
    *datum = state->lasto;

    // net refcount change of 0
  }

  state->prev_total = 0;
  
  return SMACQ_PASS|SMACQ_END;
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_entropy_table = {
  &entropy_produce, 
  &entropy_consume,
  &entropy_init,
  &entropy_shutdown
};
