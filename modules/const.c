#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "smacq.h"

struct state {
  smacq_environment * env;
  dts_field field;
  const dts_object * data;
};

static struct smacq_options options[] = {
  {NULL, {NULL}, NULL, 0}
};

static smacq_result const_consume(struct state * state, const dts_object * datum, int * outchan) {
  assert(datum);

  dts_attach_field(datum, state->field, state->data);
  return SMACQ_PASS;
}

static int const_init(struct smacq_init * context) {
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

  assert(argc<=2);
  assert(argc>=1);

  state->data = smacq_construct_fromstring(state->env, smacq_requiretype(state->env, "string"), strdup(argv[0]));
  if (argc == 2) {
  	state->field = smacq_requirefield(state->env, argv[1]);
  } else {
  	state->field = smacq_requirefield(state->env, argv[0]);
  }

  return SMACQ_PASS;
}

static int const_shutdown(struct state * state) {
  dts_field_free(state->field);
  free(state);
  return SMACQ_END;
}

struct smacq_functions smacq_const_table = {
	produce: NULL,
	consume: &const_consume,
	init: &const_init,
	shutdown: &const_shutdown,
};

