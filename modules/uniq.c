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
  double prob; // Use probabilistic algebraorithms?
};

static struct smacq_options options[] = {
  {"m", {double_t:0}, "Max amount of memory (MB) (forces probabilistic mode)", SMACQ_OPT_TYPE_DOUBLE},
  {NULL, {string_t:NULL}, NULL, 0}
};

/*
 * Check presense in set.
 */
static smacq_result uniq_consume(struct state * state, const dts_object * datum, int * outchan) {
  struct iovec * domainv = fields2vec(state->env, datum, &state->fieldset);

  if (!domainv) {
    //fprintf(stderr, "Skipping datum\n");
    return SMACQ_FREE;
  }

  if (!state->prob) {
    if (bytes_hash_table_insertv(state->drset, domainv, state->fieldset.num, (gpointer)1)) 
      // New entry in set
      return SMACQ_PASS;
  } else {
    if (bloom_insertv(state->summary, domainv, state->fieldset.num)) 
      return SMACQ_PASS;
  }
    
  return SMACQ_FREE;
}

static int uniq_init(struct smacq_init * context) {
  int argc;
  char ** argv;
  struct state * state = context->state = g_new0(struct state, 1);
  state->env = context->env;

  {
	smacq_opt prob;

  	struct smacq_optval optvals[] = {
    		{ "m", &prob}, 
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

	state->prob = prob.double_t;
  }

  // Consume rest of arguments as fieldnames
  fields_init(state->env, &state->fieldset, argc, argv);

  if (!state->prob) {
    state->drset = bytes_hash_table_new(KEYBYTES, CHAIN, NOFREE);
  } else {
		// summary argument is number of bits, module argument is MB
    state->summary = bloom_summary_init(KEYBYTES, state->prob * 1024 * 1024 * 8);
  }

  return 0;
}

static int uniq_shutdown(struct state * state) {
  return 0;
}

static smacq_result uniq_produce(struct state * state, const dts_object ** datum, int * outchan) {
  return SMACQ_ERROR;
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_uniq_table = {
  &uniq_produce, 
  &uniq_consume,
  &uniq_init,
  &uniq_shutdown
};
