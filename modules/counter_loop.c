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
#include <bytehash.h>

/* Programming constants */
#define KEYBYTES 128

static struct smacq_options options[] = {
  {"pdf", {boolean_t:0}, "Report probabilities rather than absolute counts", SMACQ_OPT_TYPE_BOOLEAN},
  {"f", {string_t:"counter"}, "Name of field to store count in", SMACQ_OPT_TYPE_STRING},
  {NULL, {string_t:NULL}, NULL, 0}
};

struct state {
  smacq_environment * env;
  struct fieldset fieldset;
  struct iovec_hash *counters;

  int counter;

  int prob; // Report probabilities
  
  dts_field timefield; // Field number
  dts_field probfield; 
  dts_field countfield;
  int counttype;
  int probtype;
}; 

static smacq_result count_init(struct smacq_init * context, struct state * state) {
  int argc = 0;
  char ** argv;
  smacq_opt probability, countfield;

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
  fields_init(context->env, &state->fieldset, argc, argv);

  state->timefield = smacq_requirefield(context->env, "timeseries");
  if (state->prob) {
  	state->probfield = smacq_requirefield(context->env, "probability");
  	state->probtype = smacq_requiretype(context->env, "double");
  } else {
  	state->countfield = smacq_requirefield(context->env, countfield.string_t);
  	state->counttype = smacq_requiretype(context->env, "int");
  }

  state->counters = bytes_hash_table_new(KEYBYTES, CHAIN, NOFREE);

  return 0;
}

static smacq_result count_thread(struct smacq_init * context) {
  const dts_object * datum;
  struct state state;
  int c;

  memset(&state, 0, sizeof(struct state));
  
  c = ++state.counter;

  count_init(context, &state);

  //fprintf(stderr, "count thread is running\n");

  while( (datum = smacq_read(context)) ) {
    //fprintf(stderr, "count thread got datum\n");

    if (state.fieldset.num) {
      struct iovec * domainv = fields2vec(context->env, datum, &state.fieldset);

      if (!domainv) {
        //fprintf(stderr, "Skipping datum\n");
        return SMACQ_FREE;
      }

      c = bytes_hash_table_incrementv(state.counters, domainv, state.fieldset.num);
      c++;
    }

    if (state.prob) {
      double p = (double)c / state.counter;
      dts_object * msgdata = smacq_dts_construct(context->env, state.probtype, &p);
      dts_attach_field(datum, state.probfield, msgdata); 
    } else {
      dts_object * msgdata = smacq_dts_construct(context->env, state.counttype, &c);
      dts_attach_field(datum, state.countfield, msgdata); 
    }
   
    smacq_decision(context, datum, SMACQ_PASS);
  }

  /* Shutdown */
  bytes_hash_table_destroy(state.counters);

  //fprintf(stderr, "Count thread exiting\n");

  return 0;
}

struct smacq_functions smacq_counter_loop_table = SMACQ_THREADED_MODULE(count_thread);
