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

static smacq_result print_consume(struct state * state, const dts_object * datum, int * outchan) {
  int i,j;
  int printed = 0;
  const dts_object *field;
  assert(datum);

  for (i = 0; i < state->argc; i++) {
    if (printed) {
      	printf(state->delimiter);
    }
    if ((field = smacq_getfield(state->env, datum, state->fields[i], NULL))) {
        if (!printed) {
    	   for (j=0; j<i; j++) 
    		printf(state->delimiter);
        }
        printed++;
	if (state->verbose) {
		printf("%.20s = %s", state->argv[i], (char *)dts_getdata(field));
	} else {
		printf("%s", (char*)dts_getdata(field));
	}
        dts_decref(field);
    } else if (state->verbose) {
      fprintf(stderr, "Warning: print: no field %s.string\n", state->argv[i]);
    }
  }
  if (printed) {
 	printf("\n");
  }
  if (state->flush) fflush(stdout); 
  return SMACQ_PASS;
}

static int print_init(struct smacq_init * context) {
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
	  state->fields[i] = smacq_requirefield(state->env, dts_fieldname_append(state->argv[i],"string")); 
  }
  return 0;
}

static int print_shutdown(struct state * state) {
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

