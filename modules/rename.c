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
}; 
 
static smacq_result rename_consume(struct state * state, const dts_object * datum, int * outchan) {
  int i;

  for (i = 0; i < state->fieldset.num; i+=2) {
	dts_object * newf = (dts_object*)smacq_alloc(state->env, 0, 0);
  	if (!smacq_getfield(state->env, datum, state->fieldset.fields[i].num, newf)) {
		dts_decref(newf);
		fprintf(stderr, "rename: no %s field\n", 
				state->fieldset.fields[i].name);
	}

    	dts_attach_field(datum, state->fieldset.fields[i+1].num, newf); 
	dts_incref(newf, 1);
  }

  return SMACQ_PASS;
}

static int rename_init(struct smacq_init * context) {
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

  assert(argc && ((argc%2) == 0));
  fields_init(state->env, &state->fieldset, argc, argv);

  return 0;
}

static int rename_shutdown(struct state * state) {
  return 0;
}


static smacq_result rename_produce(struct state * state, const dts_object ** datum, int * outchan) {
  return SMACQ_END;
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_rename_table = {
  &rename_produce, 
  &rename_consume,
  &rename_init,
  &rename_shutdown
};
