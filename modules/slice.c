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

/* Programming constants */

#define VECTORSIZE 21
#define ALARM_BITS 15
#define KEYBYTES 128

static struct smacq_options options[] = {
  {"s", {string_t:NULL}, "Start point", SMACQ_OPT_TYPE_STRING},
  {"e", {string_t:NULL}, "End point", SMACQ_OPT_TYPE_STRING},
  {NULL, {string_t:NULL}, NULL, 0}
};

struct state {
  smacq_environment* env
  
  void * startp, * endp;
  int startlen, endlen;
  int starttype, endtype;
}; 



static void printout(struct state * state) {
  if (!state->fieldset.num) { 
    if (state->hasinterval)
      printf("%lu.%06lu\t%lu\n", state->nextinterval.tv_sec, state->nextinterval.tv_usec, state->counter);
    else 
      printf("%lu\n", state->counter);

  } else {
    if (state->hasinterval) 
      printf("\fCounters at %lu.%06lu\n", state->nextinterval.tv_sec, state->nextinterval.tv_usec);

    bytes_hash_table_foreach(state->counters, print_counter, state);
  }
}
  
static smacq_result slice_consume(struct state * state, const dts_object * datum, int * outchan) {
  int c;

  if (state->hasinterval) {
    int len, type;
    struct timeval * value;

    if (!flow_getfield(state->env, datum, "timeseries", &type, (void**)&value, &len)) {
      fprintf(stderr, "error: timeseries not available\n");
    } else {
      assert(len == sizeof(struct timeval));
      
      if (!state->istarted) {
	state->istarted = 1;
	state->nextinterval = *value;
	timeval_inc(&state->nextinterval, state->interval);
      } else if (timeval_ge(*value, state->nextinterval)) {
	// Print counters
	printout(state);

	timeval_inc(&state->nextinterval, state->interval);
	while (timeval_past(*value, state->nextinterval)) { // gap in timeseries
	  timeval_inc(&state->nextinterval, state->interval);
	}
      }
    }
  }

  if (!state->fieldset.num) {
    state->counter++;
  } else {
    struct iovec * domainv = fields2vec(state->env, datum, &state->fieldset);

    if (!domainv) {
      //fprintf(stderr, "Skipping datum\n");
      return SMACQ_FREE;
    }

    c = bytes_hash_table_incrementv(state->counters, domainv, state->fieldset.num);
  }

  return SMACQ_PASS;
}

static int slice_init(struct flow_init * context) {
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
  	flow_getoptsbyname(context->argc-1, context->argv+1,
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
  fields_init(&state->fieldset, argc, argv);

  state->counters = bytes_hash_table_new(KEYBYTES, chain);

  return 0;
}

static int slice_shutdown(struct state * state) {
  // Print counters
  printout(state);
  return 0;
}


static smacq_result slice_produce(struct state * state, const dts_object ** datum, int * outchan) {
  return SMACQ_ERROR;
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_slice_table = {
  &slice_produce, 
  &slice_consume,
  &slice_init,
  &slice_shutdown
};
