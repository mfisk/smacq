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

/* Programming constants */

#define VECTORSIZE 21
#define ALARM_BITS 15
#define KEYBYTES 128

static struct smacq_options options[] = {
  {NULL, {string_t:NULL}, NULL, 0}
};

struct obj_list{
  const dts_object * obj;
  struct obj_list * next;
};

struct state {
  smacq_environment * env;
  struct fieldset fieldset;
  struct iovec_hash *last;
  
  struct obj_list * outputq, *list;

  int refreshtype, probtype;
  dts_field probfield, countfield;
  unsigned long long total;
}; 

static void compute_all(struct state * state) {
  const dts_object * count; 
  dts_object * pfield; 
  struct obj_list * n;
  double p;

  state->outputq = state->list;
  state->list = NULL;

  for (n = state->outputq; n; n = n->next) {
  	if (! (count = smacq_getfield(state->env, n->obj, state->countfield, NULL))) {
		assert(0);
	}
	p = (double)(dts_data_as(count, int)) / (double)(state->total);
	//fprintf(stderr, "%d / %lld = %g\n", *(int*)(count.data), state->total, p);
	pfield = smacq_dts_construct(state->env, state->probtype, &p);
	dts_attach_field(n->obj, state->probfield, pfield); 
	dts_decref(count);
  }

  state->total = 0;
}
  
static smacq_result pdf_consume(struct state * state, const dts_object * datum, int * outchan) {
  const dts_object * count; 
  smacq_result res = SMACQ_FREE;

  if (! (count = smacq_getfield(state->env, datum, state->countfield, NULL))) {
      if (dts_gettype(datum) == state->refreshtype) {
          compute_all(state);
	  res = SMACQ_PASS;
      } else {
      	fprintf(stderr, "error: timeseries not available\n");
      	return SMACQ_PASS;
      }
  } else {
  	struct obj_list * newo = g_new(struct obj_list, 1);
  	newo->obj = datum;
  	newo->next = state->list;
  	state->list = newo;

	dts_incref(datum, 1);

  	state->total += dts_data_as(count, int);
	dts_decref(count);
  }

  if (state->outputq) res |= SMACQ_PRODUCE;

  return(res);
}

static smacq_result pdf_init(struct smacq_init * context) {
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

  state->countfield = smacq_requirefield(state->env, "count");
  state->probfield = smacq_requirefield(state->env, "probability");
  state->probtype = smacq_requiretype(state->env, "double");
  state->refreshtype = smacq_requiretype(state->env, "refresh");

  return 0;
}

static smacq_result pdf_shutdown(struct state * state) {
  return 0;
}


static smacq_result pdf_produce(struct state * state, const dts_object ** datum, int * outchan) {
  if (!state->outputq) {
    compute_all(state);
  }
    
  if (state->outputq) {
    *datum = state->outputq->obj;
    state->outputq = state->outputq->next;
  } else {
    return SMACQ_END;
  }


  return(SMACQ_PASS|(state->outputq ? SMACQ_PRODUCE : 0));
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_pdf_table = {
  &pdf_produce, 
  &pdf_consume,
  &pdf_init,
  &pdf_shutdown
};
