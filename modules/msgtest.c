#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <smacq.h>

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

#include <smacq-parser.h>
/* 
 * This is abstraction violation.  comp_new() is currently in a different .h file.
 * We don't provide a string constant and instead prime the comparison object cache.
 */
static smacq_result msgtest_consume(struct state * state, const dts_object * datum, int * outchan) {
  dts_comparison * comp = comp_new(EQ, comp_operand(FIELD, "srcip"), comp_operand(CONST, ""));
  const dts_object * msgdata, * srcip;

  // datum = dts_writable(state->env, datum);
  
  /* Get current time as message data */
  msgdata = smacq_getfield(state->env, datum,
		smacq_requirefield(state->env, "timeseries"), 
		NULL);

  /* Get current address as matching criteria (msg destination) */
  srcip = smacq_getfield(state->env, datum, smacq_requirefield(state->env, "srcip"), NULL);
  comp->op2->valueo = dts_dup(state->env->types, srcip);
  dts_decref(srcip);

  /* Send it to everybody else */
  smacq_msg_send(state->env, smacq_requirefield(state->env, "prior"), dts_dup(state->env->types, msgdata), comp);

  dts_decref(msgdata);

  return SMACQ_PASS;
}

static smacq_result msgtest_init(struct smacq_init * context) {
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

struct smacq_functions smacq_msgtest_table = {
  produce: &msgtest_produce, 
  consume: &msgtest_consume,
  init: &msgtest_init,
};


