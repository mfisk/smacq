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

static smacq_result filter_produce(struct state* state, const dts_object ** datum, int * outchan) {
  return SMACQ_ERROR;
}

static smacq_result filter_consume(struct state * state, const dts_object * datum, int * outchan) {
  int same_types;
  
  assert(datum);
  assert(state->argv[0]);
  
  if (dts_gettype(datum) != state->type) {
    same_types = 0;
  } else {
    same_types = 1;
  }

  if (flow_match(state->env, datum, state->comp, same_types)) 
    return SMACQ_PASS;
  else
    return SMACQ_FREE;
}

static int filter_init(struct flow_init * context) {
  struct state * state;
  int i;

  context->state = state = (struct state*) calloc(sizeof(struct state),1);
  assert(state);

  state->env = context->env;
  {
    struct smacq_optval optvals[] = {
      {NULL, NULL}
    };
    flow_getoptsbyname(context->argc-1, context->argv+1,
				 &state->argc, &state->argv,
				 options, optvals);
    assert(state->argv);
  }

  state->comp = g_new0(dts_comparison, state->argc);

  for (i=0; i < state->argc; i++) {
    // Make array a linked list too
    if (i) state->comp[i-1].next = state->comp+i;

    flow_parsetest(state->env, state->comp+i, state->argv[i]);
  }

  return 0;
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_filter_table = {
  &filter_produce, 
  &filter_consume,
  &filter_init,
  NULL
};

