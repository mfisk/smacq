#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
#include <fields.h>

static struct smacq_options options[] = {
  {NULL, {string_t:NULL}, NULL, 0}
};

struct state {
  smacq_environment * env;
  struct fieldset fieldset;
  int empty_type;
  dts_object * product;
}; 
 
static smacq_result project_consume(struct state * state, const dts_object * datum, int * outchan) {
  dts_object * newo;
  int i;

  newo = smacq_dts_construct(state->env, state->empty_type, NULL);
  assert(newo);

  for (i = 0; i < state->fieldset.num; i++) {
	dts_object * newf = (dts_object*)smacq_alloc(state->env, 0, 0);

  	if (!smacq_getfield(state->env, datum, state->fieldset.fields[i].num, newf)) {
		fprintf(stderr, "project: no %s field\n", 
				state->fieldset.fields[i].name);
		dts_decref(newf);
		continue;
	}
    	dts_attach_field(newo, state->fieldset.fields[i].num, newf); 
	dts_incref(newf, 1);
  }

  state->product = newo;
  return SMACQ_FREE|SMACQ_PRODUCE;
}

static int project_init(struct smacq_init * context) {
  int argc = 0;
  char ** argv;
  struct state * state = context->state = g_new0(struct state, 1);
  state->env = context->env;

  {
  	struct smacq_optval optvals[] = {
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);
  }

  assert(argc>=1);
  fields_init(state->env, &state->fieldset, argc, argv);

  state->empty_type = smacq_requiretype(state->env, "empty");

  return 0;
}

static int project_shutdown(struct state * state) {
  return 0;
}


static smacq_result project_produce(struct state * state, const dts_object ** datum, int * outchan) {
  if (state->product) {
	  *datum = state->product;
	  state->product = NULL;
	  return SMACQ_PASS;
  }
  return SMACQ_END;
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_project_table = {
  &project_produce, 
  &project_consume,
  &project_init,
  &project_shutdown
};
