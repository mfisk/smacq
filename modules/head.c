#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "smacq.h"

struct state {
  smacq_environment * env;
  int left;
};

static struct smacq_options options[] = {
  {NULL, {string_t:NULL}, NULL, 0}
};

static smacq_result head_produce(struct state* state, const dts_object ** datum, int * outchan) {
  return SMACQ_ERROR;
}

static smacq_result head_consume(struct state * state, const dts_object * datum, int * outchan) {
  assert(datum);

  if (state->left-- > 0) return SMACQ_PASS;

  return SMACQ_END;
}

static smacq_result head_init(struct smacq_init * context) {
  struct state * state;
  int argc;
  char ** argv;

  context->state = state = (struct state*) calloc(sizeof(struct state),1);
  assert(state);

  state->env = context->env;
  {
    struct smacq_optval optvals[] = {
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &argc, &argv,
				 options, optvals);
  }

  assert(argc);

  state->left = atoi(argv[0]);

  return 0;
}


/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_head_table = {
  produce: &head_produce, 
  consume: &head_consume,
  init: &head_init,
  algebra: { nesting: 1},
};

