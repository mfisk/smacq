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
  {"pdf", {boolean_t:0}, "Report probabilities rather than absolute counts", SMACQ_OPT_TYPE_BOOLEAN},
  {"f", {string_t:"counter"}, "Name of field to store count in", SMACQ_OPT_TYPE_STRING},
  {NULL, {string_t:NULL}, NULL, 0}
};

struct state {
  smacq_environment * env;
  struct fieldset fieldset;
  GHashTableofBytes *counters;

  int counter;

  int prob; // Report probabilities
  
  int timefield; // Field number
  int probfield; 
  int countfield;
  int counttype;
  int probtype;
}; 
  
static smacq_result count_consume(struct state * state, const dts_object * datum, int * outchan) {
  int c = ++state->counter;


  if (state->fieldset.num) {
    struct iovec * domainv = fields2vec(state->env, datum, &state->fieldset);

    if (!domainv) {
      //fprintf(stderr, "Skipping datum\n");
      return SMACQ_FREE;
    }

    c = bytes_hash_table_incrementv(state->counters, domainv, state->fieldset.num);
    c++;
  }

  if (state->prob) {
    double p = (double)c / state->counter;
    dts_object * msgdata = flow_dts_construct(state->env, state->probtype, &p);
    dts_attach_field(datum, state->probfield, msgdata); 
  } else {
    dts_object * msgdata = flow_dts_construct(state->env, state->counttype, &c);
    dts_attach_field(datum, state->countfield, msgdata); 
  }
 
  return SMACQ_PASS;
}

static int count_init(struct smacq_init * context) {
  int argc = 0;
  char ** argv;
  smacq_opt probability, countfield;
  struct state * state = context->state = g_new0(struct state, 1);
  state->env = context->env;

  {
  	struct smacq_optval optvals[] = {
		{"pdf", &probability},
		{"f", &countfield},
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

	state->prob = probability.boolean_t;
  }

  // Consume rest of arguments as fieldnames
  fields_init(state->env, &state->fieldset, argc, argv);

  state->timefield = flow_requirefield(state->env, "timeseries");
  if (state->prob) {
  	state->probfield = flow_requirefield(state->env, "probability");
  	state->probtype = flow_requiretype(state->env, "double");
  } else {
  	state->countfield = flow_requirefield(state->env, countfield.string_t);
  	state->counttype = flow_requiretype(state->env, "int");
  }

  state->counters = bytes_hash_table_new(KEYBYTES, CHAIN, NOFREE);

  return 0;
}

static int count_shutdown(struct state * state) {
  bytes_hash_table_destroy(state->counters);
  free(state);
  // Print counters
  return 0;
}


static smacq_result count_produce(struct state * state, const dts_object ** datum, int * outchan) {
  return SMACQ_ERROR;
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_counter_table = {
  &count_produce, 
  &count_consume,
  &count_init,
  &count_shutdown
};
