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
  char * fieldname;
  struct smacq_outputq * outputq;
};

static struct smacq_options options[] = {
  {"f", {string_t:NULL}, "Field to inspect (full data is default)", SMACQ_OPT_TYPE_STRING},
  {"m", {boolean_t:0}, "OR multiple fields and demux to individual outputs", SMACQ_OPT_TYPE_BOOLEAN},
  {NULL, {NULL}, NULL, 0}
};

static void add_entry(struct state * state, char * field, char * needle, int output) {
  if (!field && !needle) return;

  if (!needle) {
  	needle = field;
	field = NULL;
  }

  if (field) {
	if (!state->fieldname) {
    		state->field = smacq_requirefield(state->env, field);
		state->fieldname = field;
	} else if (strcmp(state->fieldname, field)) {
		/* XXX: Uh-oh, we don't know how to search multiple fields */
		assert(!"Cannot mix field names");
	}
  }

#ifdef DEBUG
  fprintf(stderr, "searching for '%s'\n", needle);
#endif
  substr_add(state->set, strlen(needle), needle, 0, (void*)output, 0, 0);
}

static smacq_result substr_produce(struct state* state, const dts_object ** datum, int * outchan) {
  return smacq_produce_dequeue(&state->outputq, datum, outchan);
}

static smacq_result substr_consume(struct state * state, const dts_object * datum, int * outchan) {
  struct substr_search_result res;
  const dts_object * field;
  int matched = 0;
  int chan;

  assert(datum);

  if (state->field) {
  	field = smacq_getfield(state->env, datum, state->field, NULL);
  	if (!field) return SMACQ_FREE;
  } else {
        field = datum;
  }
  
  memset(&res, 0, sizeof(res));

  while(1) {
	  if (! substr_search(state->set, field->data, field->len, &res)) 
		  break;

	  chan = (int)res.p->handle;

#ifdef DEBUG
	  fprintf(stderr, "pattern match '%.*s' at offset %d output %d\n", res.p->len, res.p->pattern, res.shift, chan);
#endif

	  if (matched) {
		smacq_produce_enqueue(&state->outputq, datum, chan);
		dts_incref(datum, 1);
	  } else {
	  	matched = 1;
		*outchan = chan;
	  }
  }

  if (state->field) 
	  dts_decref(field);

  if (matched) return SMACQ_PASS;

  return SMACQ_FREE;
}

static int substr_init(struct smacq_init * context) {
  struct state * state;
  int i, argc;
  char ** argv;
  smacq_opt field, demux;
  char * a = NULL;
  char * b = NULL;
  int output = 0;

  context->state = state = (struct state*) calloc(sizeof(struct state),1);
  assert(state);

  state->env = context->env;
  {
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
	  if (!strcmp(argv[i], ";")) {
		add_entry(state, a, b, output++);
		a = NULL;
		b = NULL;
	  } else if (!a) {
		a = argv[i];
	  } else {
		b = argv[i];
	  }
  }
  add_entry(state, a, b, output++);

  if (output>1) {
	  state->demux = 1;
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
	alg: {vector: 1, boolean: 1}
};

