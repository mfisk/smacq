#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "smacq.h"
#include "substr/substr.h"

struct state {
  smacq_environment * env;
  dts_field field;
  struct ruleset * set;
  int demux;
};

static struct smacq_options options[] = {
  {"f", {string_t:NULL}, "Field to inspect (full data is default)", SMACQ_OPT_TYPE_STRING},
  {"m", {boolean_t:0}, "OR multiple fields and demux to individual outputs", SMACQ_OPT_TYPE_BOOLEAN},
  {NULL, {NULL}, NULL, 0}
};

static smacq_result substr_produce(struct state* state, const dts_object ** datum, int * outchan) {
  return SMACQ_ERROR;
}

static smacq_result substr_consume(struct state * state, const dts_object * datum, int * outchan) {
  struct substr_search_result res;
  const dts_object * field;
  int found;

  int free_it = 1;
  assert(datum);

  if (state->field) {
  	field = smacq_getfield(state->env, datum, state->field, NULL);
  	if (!field) return SMACQ_FREE;
  } else {
        field = datum;
  }
  
  memset(&res, 0, sizeof(res));

  while(1) {
	  found = substr_search(state->set, field->data, field->len, &res);
	  if (!found) break;

	  if (state->demux) {
	  	assert(free_it); /* Not ready to handle multiple discreet output channels */

	  	*outchan = (int)res.p->handle;
	  }

	  free_it = 0;
  }
  if (state->field) 
	  //dts_decref(field);

  if (free_it) return SMACQ_FREE;

  return SMACQ_PASS;
}

static int substr_init(struct smacq_init * context) {
  struct state * state;
  int i, argc;
  char ** argv;

  context->state = state = (struct state*) calloc(sizeof(struct state),1);
  assert(state);

  state->env = context->env;
  {
    smacq_opt field, demux;

    struct smacq_optval optvals[] = {
      {"f", &field},
      {"m", &demux},
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &argc, &argv,
				 options, optvals);

    state->field = smacq_requirefield(state->env, field.string_t);
    state->demux = demux.boolean_t;
  }

  state->set = substr_new(SUBSTR_FAST);
  for (i = 0; i < argc; i++) {
	  int outchan;
	  if (state->demux) {
		  outchan = i;
	  } else {
		  outchan = -1;
	  }
	  substr_add(state->set, strlen(argv[i]), argv[i], 0, (void*)outchan, 0, 0);
  }
  substr_compile(state->set);
  return 0;
}

static int substr_shutdown(struct state * state) {
  free(state);
  return 0;
}

struct smacq_functions smacq_substr_table = {
	produce: &substr_produce, 
	consume: &substr_consume,
	init: &substr_init,
	shutdown: &substr_shutdown,
};

