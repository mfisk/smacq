#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "smacq.h"

struct state {
  smacq_environment * env;
  int argc;
  char ** argv;
};

static struct smacq_options options[] = {
  {NULL, {string_t:NULL}, NULL, 0}
};

static smacq_result msgtest_produce(struct state* state, const dts_object ** datum, int * outchan) {
  return SMACQ_ERROR;
}

static smacq_result msgtest_consume(struct state * state, const dts_object * datum, int * outchan) {
  dts_comparison * comp = g_new0(dts_comparison, 1);
  dts_object msgdata;

  // datum = dts_writable(state->env, datum);
  
  /* Get current time as message data */
  smacq_getfield_copy(state->env, datum,
		smacq_requirefield(state->env, "timeseries"), 
		&msgdata);

  /* Get current address as matching criteria (msg destination) */
  smacq_getfield_copy(state->env, datum,
		smacq_requirefield(state->env, "srcip"), 
		&comp->field_data);

  comp->op = EQ;
  comp->field = smacq_requirefield(state->env, "srcip");
  
  /* Send it to everybody else */

  smacq_msg_send(state->env, smacq_requirefield(state->env, "prior"), &msgdata, comp);

  return SMACQ_PASS;
}

static int msgtest_init(struct smacq_init * context) {
  struct state * state;

  context->state = state = (struct state*) calloc(sizeof(struct state),1);
  assert(state);

  state->env = context->env;
  {
    struct smacq_optval optvals[] = {
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &state->argc, &state->argv,
				 options, optvals);
    assert(state->argv);
  }

  return 0;
}


/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_msgtest_table = {
  &msgtest_produce, 
  &msgtest_consume,
  &msgtest_init,
  NULL
};


