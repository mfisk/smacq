/*
 * This module filters data to find the most frequent values of a field or fields
 */

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
#include <bloom.h>
#include "bytehash.h"

/* Programming constants */

#define VECTORSIZE 21
#define ALARM_BITS 15
#define KEYBYTES 128

struct state {
  smacq_environment * env;
  struct fieldset fieldset;
  GHashTableofBytes *drset;
  struct bloom_summary *summary;
  double prob; // Use probabilistic algorithms?
  int threshold;

  // Non-probabilisitic only:
  int numentries;
  int totalcount;

  int do_count;
  int do_filter;

  int count_field;
  int int_type;
};

static struct smacq_options options[] = {
  {"m", {double_t:0}, "Max amount of memory (MB) (forces probabilistic mode)", SMACQ_OPT_TYPE_DOUBLE},
  {"r", {double_t:1}, "Minimum ratio to average", SMACQ_OPT_TYPE_DOUBLE},
  {"f", {string_t:"pcount"}, "Count field", SMACQ_OPT_TYPE_STRING},
  {NULL, {string_t:NULL}, NULL, 0}
};

/*
 * Check presense in set.
 */
static smacq_result top_consume(struct state * state, const dts_object * datum, int * outchan) {
  struct iovec * domainv = fields2vec(state->env, datum, &state->fieldset);
  double deviation;
  int val;

  if (!domainv) {
    //fprintf(stderr, "Skipping datum\n");
    return SMACQ_FREE;
  }

  if (!state->prob) {
    val = bytes_hash_table_incrementv(state->drset, domainv, state->fieldset.num);

    if (val == 0) state->numentries++;   // New entry
    
    state->totalcount++;
  } else {
    val = bloom_incrementv(state->summary, domainv, state->fieldset.num);
  }

  /* Move val from old value to new value */
  val++;

  if (state->do_count) {
    dts_object * msgdata = smacq_dts_construct(state->env, state->int_type, &val);
    dts_attach_field(datum, state->count_field, msgdata);   
  }

  if (!state->do_filter) return SMACQ_PASS;

  /* Compute deviation */
  if (state->prob) {
        deviation = bloom_deviation(state->summary, val);
  } else {
        deviation = (double)(val) * (double)state->numentries / (double)state->totalcount;
  }

  if (deviation > state->threshold) {
    //printf("info: top: deviation is %g\n", deviation);
    return SMACQ_PASS;
  } else {
    return SMACQ_FREE;
  }
}

static int top_init(struct smacq_init * context) {
  int argc;
  char ** argv;
  smacq_opt pcount;
  struct state * state = context->state = g_new0(struct state, 1);
  state->env = context->env;

  {
	smacq_opt prob, thresh;

  	struct smacq_optval optvals[] = {
    		{ "m", &prob}, 
    		{ "r", &thresh}, 
		{ "f", &pcount},
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

	state->prob = prob.double_t;
	state->threshold = thresh.double_t;
  }

  // Consume rest of arguments as fieldnames
  fields_init(state->env, &state->fieldset, argc, argv);

  if (!state->prob) {
    state->drset = bytes_hash_table_new(KEYBYTES, CHAIN, NOFREE);
  } else {
    state->summary = bloom_counter_init(KEYBYTES, state->prob/4 * 1024 * 1024);
  }

  state->do_filter = (state->threshold == 0);

  if (strcmp(pcount.string_t,  "")) {
    state->count_field = smacq_requirefield(state->env, pcount.string_t);
    state->int_type = smacq_requiretype(state->env, "int");
    state->do_count = 1;
  } else {
    state->do_count = 0;
  }

  return 0;
}

static int top_shutdown(struct state * state) {
  return 0;
}

static smacq_result top_produce(struct state * state, const dts_object ** datum, int * outchan) {
  return SMACQ_ERROR;
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_top_table = {
  &top_produce, 
  &top_consume,
  &top_init,
  &top_shutdown
};
