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
  {"pdf", {boolean_t:0}, "Report probabilities rather than absolute counts", SMACQ_OPT_TYPE_BOOLEAN},
  {"a", {boolean_t:0}, "Annotate and pass all objects instead of just the last", SMACQ_OPT_TYPE_BOOLEAN},
  {"f", {string_t:"count"}, "Name of field to store count in", SMACQ_OPT_TYPE_STRING},
  {NULL, {string_t:NULL}, NULL, 0}
};

struct state {
  smacq_environment * env;
  struct fieldset fieldset;
  struct iovec_hash *counters;

  int counter;

  int prob; // Report probabilities

  int all;  // -a flag
  const dts_object * lastin;
  
  dts_field timefield; // Field number
  dts_field probfield; 
  dts_field countfield;
  int counttype;
  int probtype;
}; 
  
static void annotate(struct state * state, const dts_object * datum, int c) {
  if (state->prob) {
    double p = (double)c / state->counter;
    dts_object * msgdata = smacq_dts_construct(state->env, state->probtype, &p);
    dts_attach_field(datum, state->probfield, msgdata); 
  } else {
    dts_object * msgdata = smacq_dts_construct(state->env, state->counttype, &c);
    dts_attach_field(datum, state->countfield, msgdata); 
  }
}
 
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

  if (!state->all) {
    if (state->lastin) {
	    dts_decref(state->lastin);
    }
    dts_incref(datum, 1);
    state->lastin = datum;

    return SMACQ_FREE;
  } 

  annotate(state, datum, c);

  return SMACQ_PASS;
}

static smacq_result count_init(struct smacq_init * context) {
  int argc = 0;
  char ** argv = NULL;
  smacq_opt probability, countfield, allflag;
  struct state * state = context->state = g_new0(struct state, 1);
  state->env = context->env;

  {
  	struct smacq_optval optvals[] = {
		{"pdf", &probability},
		{"f", &countfield},
		{"a", &allflag},
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

	state->prob = probability.boolean_t;
	state->all = allflag.boolean_t;
  }

  // Consume rest of arguments as fieldnames
  fields_init(state->env, &state->fieldset, argc, argv);

  state->timefield = smacq_requirefield(state->env, "timeseries");
  if (state->prob) {
  	state->probfield = smacq_requirefield(state->env, "probability");
  	state->probtype = smacq_requiretype(state->env, "double");
  } else {
  	state->countfield = smacq_requirefield(state->env, countfield.string_t);
  	state->counttype = smacq_requiretype(state->env, "int");
  }

  if (state->fieldset.num) 
  	state->counters = bytes_hash_table_new(KEYBYTES, CHAIN|NOFREE);

  if (argv) free(argv);
  return 0;
}

static smacq_result count_shutdown(struct state * state) {
  //fprintf(stderr, "count_shutdown!\n");
  //
  if (state->prob) {
  	dts_field_free(state->probfield);
  } else {
  	dts_field_free(state->countfield);
  }

  if (state->fieldset.num) 
  	bytes_hash_table_destroy(state->counters);

  fieldset_destroy(&state->fieldset);
  free(state);

  return 0;
}


static smacq_result count_produce(struct state * state, const dts_object ** datump, int * outchan) {
  int c;
  struct iovec * domainv = fields2vec(state->env, state->lastin, &state->fieldset);

  if (!state->lastin) {
	return SMACQ_FREE;
  }

  *datump = state->lastin;
  state->lastin = NULL;

  if (state->fieldset.num) {
	assert("count: If field names are specified, you must use -a\n!");
  	c = (int)bytes_hash_table_lookupv(state->counters, domainv, state->fieldset.num);
  } else {
	c = state->counter;
  }

  assert(c!=0);
  annotate(state, *datump, c);

  //fprintf(stderr, "count_produce() %p\n", *datump);
  return SMACQ_PASS|SMACQ_END;
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_count_table = {
	produce: &count_produce, 
	consume: &count_consume,
	init: &count_init,
	shutdown: &count_shutdown
};
