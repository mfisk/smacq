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

static smacq_result noop_consume(struct state * state, const dts_object * datum, int * outchan) {
  assert(datum);

  return SMACQ_FREE;
}

static smacq_result noop_init(struct smacq_init * context) {
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

  return 0;
}


struct smacq_functions smacq_noop_table = {
  consume: &noop_consume,
  init: &noop_init,
  NULL
};

