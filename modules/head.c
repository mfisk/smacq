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

static int head_init(struct flow_init * context) {
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
    flow_getoptsbyname(context->argc-1, context->argv+1,
				 &argc, &argv,
				 options, optvals);
  }

  assert(argc);

  state->left = atoi(argv[0]);

  return 0;
}


/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_head_table = {
  &head_produce, 
  &head_consume,
  &head_init,
  NULL
};

