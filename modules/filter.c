#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <smacq.h>
#include <string.h>


struct state {
  smacq_environment * env;
  char ** argv;
  int argc;
  dts_comparison * comp;
  int type;
};

static struct smacq_options options[] = {
  {NULL, {string_t:NULL}, NULL, 0}
};

static smacq_result filter_consume(struct state * state, const dts_object * datum, int * outchan) {
  assert(datum);
  assert(state->argv[0]);
  
  state->type = dts_gettype(datum);

  if (smacq_match(state->env, datum, state->comp)) 
    return SMACQ_PASS;
  else
    return SMACQ_FREE;
}

static smacq_result filter_init(struct smacq_init * context) {
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

  state->comp = dts_parse_tests(state->env->types, state->argc, state->argv);
  if (!state->comp) 
	  return SMACQ_ERROR|SMACQ_END;

  return 0; 
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_filter_table = {
  consume: &filter_consume,
  init: &filter_init
};

