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
#include <bloom.h>
#include "bytehash.h"

/* Programming constants */

#define VECTORSIZE 21
#define ALARM_BITS 15
#define KEYBYTES 128

struct state {
  smacq_environment * env;
  struct fieldset fieldset;
  struct iovec_hash *drset;
  struct bloom_summary *summary;
  double prob; // Use probabilistic algebraorithms?
  int use_obj_id;
};

static struct smacq_options options[] = {
  {"m", {double_t:0}, "Max amount of memory (MB) (forces probabilistic mode)", SMACQ_OPT_TYPE_DOUBLE},
  {"o", {boolean_t:0}, "Use object ID instead of fields", SMACQ_OPT_TYPE_BOOLEAN},
  {NULL, {string_t:NULL}, NULL, 0}
};

/*
 * Check presense in set.
 */
static smacq_result uniq_consume(struct state * state, const dts_object * datum, int * outchan) {
  struct iovec obj_domainv[2];
  struct iovec * domainv;
  int numfields = state->fieldset.num;
  
  if (state->use_obj_id) {
	  domainv = obj_domainv;
	  obj_domainv[0].iov_base = (void*)&datum->id;
	  obj_domainv[0].iov_len = 4;
	  obj_domainv[1].iov_base = (void*)datum;
	  obj_domainv[1].iov_len = 4;
	  numfields = 2;
  } else {
	  domainv = fields2vec(state->env, datum, &state->fieldset);
  }

  if (!domainv) {
    //fprintf(stderr, "Skipping datum\n");
    return SMACQ_FREE;
  }

  if (!state->prob) {
    if (!bytes_hash_table_setv(state->drset, domainv, numfields, (void*)1)) 
      // New entry in set
      return SMACQ_PASS;
  } else {
    if (bloom_insertv(state->summary, domainv, numfields)) 
      return SMACQ_PASS;
  }
    
  return SMACQ_FREE;
}

static smacq_result uniq_init(struct smacq_init * context) {
  int argc;
  char ** argv;
  struct state * state = context->state = g_new0(struct state, 1);
  state->env = context->env;

  {
	smacq_opt prob, obj;

  	struct smacq_optval optvals[] = {
    		{ "m", &prob}, 
    		{ "o", &obj}, 
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

	state->prob = prob.double_t;
	state->use_obj_id = obj.boolean_t;
  }

  // Consume rest of arguments as fieldnames
  fields_init(state->env, &state->fieldset, argc, argv);

  if (!state->prob) {
    state->drset = bytes_hash_table_new(KEYBYTES, CHAIN|NOFREE);
  } else {
		// summary argument is number of bits, module argument is MB
    state->summary = bloom_summary_init(KEYBYTES, state->prob * 1024 * 1024 * 8);
  }

  return 0;
}

static smacq_result uniq_shutdown(struct state * state) {
  if (!state->prob) {
    bytes_hash_table_destroy(state->drset);
  } else {
    //XXX: bloom_summary_destroy(state->summary);
  }

  fieldset_destroy(&state->fieldset);

  free(state);

  return SMACQ_END;
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_uniq_table = {
	shutdown: &uniq_shutdown, 
	consume: &uniq_consume,
	init: &uniq_init,
};
