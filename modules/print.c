#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "smacq.h"

struct state {
  smacq_environment * env;
  char ** argv;
  int argc;
  int verbose;
  int flush;
  dts_field * fields;
  dts_field string_transform;
  char * delimiter;
};

static struct smacq_options options[] = {
  {"v", {boolean_t:0}, "Verbose mode: print field names", SMACQ_OPT_TYPE_BOOLEAN},
  {"d", {string_t:"\t"}, "Delimiter", SMACQ_OPT_TYPE_STRING},
  {"B", {boolean_t:0}, "Disable buffering: flush output after each line", SMACQ_OPT_TYPE_BOOLEAN},
  {NULL, {string_t:NULL}, NULL, 0}
};

static smacq_result print_produce(struct state* state, const dts_object ** datum, int * outchan) {
  return SMACQ_ERROR;
}

static int print_field(struct state * state, const dts_object * field, char * fname, int printed, int column) {
        if (printed) {
      	   printf(state->delimiter);
    	} else {
	   int j;
    	   for (j=0; j<column; j++) 
    		printf(state->delimiter);
        }

	if (state->verbose) {
		printf("%.20s = %s", fname, (char *)dts_getdata(field));
	} else {
		printf("%s", (char*)dts_getdata(field));
	}
        dts_decref(field);

	return 1;
}

static smacq_result print_consume(struct state * state, const dts_object * datum, int * outchan) {
  int i,j;
  int printed = 0;
  int column = 0;
  const dts_object *field;
  assert(datum);

  for (i = 0; i < state->argc; i++) {
    if (!state->fields[i]) {
	int j;

	dts_prime_all_fields(state->env->types, datum);

	for (j = 0; j <= datum->fields.max; j++) {
		field = dts_getfield_single(state->env->types, datum, j);
		if (field) {
			printed = print_field(state, field, "FOO", printed, column);
			column++;
		}
	}
	column--;

    } else if ((field = smacq_getfield(state->env, datum, state->fields[i], NULL))) {
	printed = print_field(state, field, state->argv[i], printed, column);

    } else if (state->verbose) {
        fprintf(stderr, "Warning: print: no field %s.string\n", state->argv[i]);
    }
    column++;
  }
  if (printed) {
 	printf("\n");
  }
  if (state->flush) fflush(stdout); 
  return SMACQ_PASS;
}

static smacq_result print_init(struct smacq_init * context) {
  struct state * state;
  smacq_opt verbose, flush, delimiter;
  int i;

  context->state = state = (struct state*) calloc(sizeof(struct state),1);
  assert(state);

  state->env = context->env;
  {
    struct smacq_optval optvals[] = {
      {"v", &verbose},
      {"d", &delimiter},
      {"B", &flush},
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &state->argc, &state->argv,
				 options, optvals);
    assert(state->argv);
  }

  state->flush = flush.boolean_t;
  state->delimiter = delimiter.string_t;
  state->verbose = verbose.boolean_t;
  state->fields = malloc(state->argc * sizeof(dts_field));
  state->string_transform = smacq_requirefield(state->env, "string");

  for (i = 0; i < state->argc; i++) {
	  if (!strcmp(state->argv[i], "*")) {
		state->fields[i] = NULL;
	  } else {
	  	state->fields[i] = smacq_requirefield(state->env, dts_fieldname_append(state->argv[i],"string")); 
	  }
  }
  return 0;
}

static smacq_result print_shutdown(struct state * state) {
  int i;

  for (i = 0; i < state->argc; i++) 
	  dts_field_free(state->fields[i]);

  free(state->fields);
  free(state);
  return 0;
}




/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_print_table = {
	produce: &print_produce, 
	consume: &print_consume,
	init: &print_init,
	shutdown: &print_shutdown,
};

