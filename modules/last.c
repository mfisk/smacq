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
#define KEYBYTES 128

static struct smacq_options options[] = {
  {"t", {double_t:0}, "Update interval", SMACQ_OPT_TYPE_TIMEVAL},
  {NULL, {string_t:NULL}, NULL, 0}
};

struct state {
  smacq_environment * env;
  struct fieldset fieldset;
  struct iovec_hash *last;
  
  struct timeval interval, nextinterval;
  int istarted, hasinterval;

  struct smacq_outputq * outputq;

  dts_field timeseries; // Field number
  int refreshtype, timevaltype;
}; 


static inline void timeval_inc(struct timeval * x, struct timeval y) {
  x->tv_usec += y.tv_usec;
  x->tv_sec += y.tv_sec;

  if (x->tv_usec > 1000000) {
    x->tv_sec++;
    x->tv_usec -= 1000000;
  }
}

static inline int timeval_ge(struct timeval x, struct timeval y) {
  if (x.tv_sec > y.tv_sec) return 1;
  if ((x.tv_sec == y.tv_sec) && (x.tv_usec >= y.tv_usec)) return 1;
  return 0;
}
  
static inline int timeval_past(struct timeval x, struct timeval y) {
  if (x.tv_sec > y.tv_sec) return 1;
  if ((x.tv_sec == y.tv_sec) && (x.tv_usec > y.tv_usec)) return 1;
  return 0;
}

static inline void timeval_minus(struct timeval x, struct timeval y, struct timeval * result) {
  *result = x;
  x.tv_sec -= y.tv_sec; // No spec for underflow

  if (y.tv_usec > x.tv_usec) {
    x.tv_sec--;
    x.tv_usec += 1e6;
  }
  x.tv_usec -= y.tv_usec;
  
  return;
}

static int emit_last(struct element * key, void * value, void * userdata) {
  dts_object * d = value;
  struct state * state = userdata;

  /* fprintf(stderr, "emit_last on obj %p\n", value); */

  smacq_produce_enqueue(&state->outputq, d, -1);
  dts_incref(d, 1);

  return 0;
}
	
static void emit_all(struct state * state) {
  assert (!state->outputq);
  bytes_hash_table_foreach(state->last, emit_last, state);

  // Last entry to be sent is a refresh message:
  if (state->fieldset.num) {
	dts_object * obj = smacq_dts_construct(state->env, state->refreshtype, NULL);

  	if (state->hasinterval) {
  		dts_object * timefield;
  		timefield = smacq_dts_construct(state->env, state->timevaltype, &state->nextinterval);
  		dts_attach_field(obj, state->timeseries, timefield);
  	}

  	smacq_produce_enqueue(&state->outputq, obj, -1);
	//fprintf(stderr, "last enqueue refresh %p\n", obj);
  }

}
  
static smacq_result last_consume(struct state * state, const dts_object * datum, int * outchan) {
  struct iovec * domainv = fields2vec(state->env, datum, &state->fieldset);
  int condproduce = 0;

  if (state->hasinterval) {
    const dts_object * field_data;

    if (!(field_data = smacq_getfield(state->env, datum, state->timeseries, NULL))) {
      fprintf(stderr, "error: timeseries not available\n");
    } else {
      struct timeval * tv = (struct timeval *)dts_getdata(field_data);
      assert(dts_getsize(field_data) == sizeof(struct timeval));
      
      if (!state->istarted) {
	state->istarted = 1;
	state->nextinterval = *tv;
	timeval_inc(&state->nextinterval, state->interval);
      } else if (timeval_ge(*tv, state->nextinterval)) {
	// Print counters
	emit_all(state);

	timeval_inc(&state->nextinterval, state->interval);
	while (timeval_past(*tv, state->nextinterval)) { // gap in timeseries
	  timeval_inc(&state->nextinterval, state->interval);
	}
      }
    }
  }

  if (state->outputq) {
    condproduce = SMACQ_PRODUCE;
  }

  if (!domainv && state->fieldset.num) {
      //fprintf(stderr, "Skipping datum\n");
      return SMACQ_PASS|condproduce;
  }

  {
	dts_object * old; 

  	dts_incref(datum, 1);
  	old = bytes_hash_table_setv(state->last, domainv, state->fieldset.num, (gpointer)datum);
	//fprintf(stderr, "last saving %p, releasing %p\n", datum, old);
	if (old) 
	  dts_decref(old);
  }

  return(SMACQ_FREE|condproduce);
}

static smacq_result last_init(struct smacq_init * context) {
  int argc = 0;
  char ** argv;
  struct state * state = context->state = g_new0(struct state, 1);
  state->env = context->env;

  {
	smacq_opt interval;

  	struct smacq_optval optvals[] = {
    		{ "t", &interval}, 
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

	state->interval = interval.timeval_t;
	if ((interval.timeval_t.tv_sec != 0) || (interval.timeval_t.tv_usec != 0)) {
	  state->hasinterval = 1;
	} else {
	  state->hasinterval = 0;
	}
  }

  // Consume rest of arguments as fieldnames
  fields_init(state->env, &state->fieldset, argc, argv);

  state->timeseries = smacq_requirefield(state->env, "timeseries");
  state->refreshtype = smacq_requiretype(state->env, "refresh");
  state->timevaltype = smacq_requiretype(state->env, "timeval");
  state->last = bytes_hash_table_new(KEYBYTES, CHAIN|NOFREE);

  return 0;
}

static smacq_result last_shutdown(struct state * state) {
  return 0;
}


static smacq_result last_produce(struct state * state, const dts_object ** datum, int * outchan) {
  if (!state->outputq) {
    emit_all(state);
  }

  return smacq_produce_dequeue(&state->outputq, datum, outchan);
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_last_table = {
  &last_produce, 
  &last_consume,
  &last_init,
  &last_shutdown
};
