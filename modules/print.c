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
  int * fields;
  int string_transform;
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
  int slen;
  char * str;
  int i;
  int printed = 0;
  dts_object field_data;
  assert(datum);
  assert(state->argv[0]);

  for (i = 0; i < state->argc; i++) {
    if (i>0)
      printf(state->delimiter);
    if (smacq_getfield(state->env, datum, state->fields[i], &field_data)) {
      int r = smacq_presentdata(state->env, &field_data, state->string_transform, (void*)&str, &slen);
      if (r == -1) {
	fprintf(stderr, "No string transform for field %s\n", state->argv[i]);
      } else if (r == 0) {
	fprintf(stderr, "Unable to transform to string\n");
      } else {
        printed++;
	if (state->verbose) {
		printf("%.20s = %s", state->argv[i], str);
	} else {
		printf("%s", str);
	}
		
	free(str);
      }
    } else if (state->verbose) {
      fprintf(stderr, "Warning: print: no field %s\n", state->argv[i]);
    }
  }
  if (printed) printf("\n");
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
  state->fields = malloc(state->argc * sizeof(int));
  state->string_transform = smacq_transform(state->env, "string");

  for (i = 0; i < state->argc; i++) 
	  state->fields[i] = smacq_requirefield(state->env, state->argv[i]); 
  return 0;
}

static int print_shutdown(struct state * state) {
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

