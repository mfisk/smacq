#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "smacq.h"

struct state {
  DTS * env;
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
  END_SMACQ_OPTIONS
};

smacq_result printModule::produce(DtsObject & datum, int & outchan) {
  return SMACQ_ERROR;
}

smacq_result printModule::consume(DtsObject datum, int & outchan) {
  int i,j;
  int printed = 0;
  DtsObjectfield;
  assert(datum);

  for (i = 0; i < argc; i++) {
    if (printed) {
      	printf(delimiter);
    }
    if ((field = datum->getfield(fields[i]))) {
        if (!printed) {
    	   for (j=0; j<i; j++) 
    		printf(delimiter);
        }
        printed++;
	if (verbose) {
		printf("%.20s = %s", argv[i], (char *)field->getdata());
	} else {
		printf("%s", (char*)field->getdata());
	}
        
    } else if (verbose) {
      fprintf(stderr, "Warning: print: no field %s.string\n", argv[i]);
    }
  }
  if (printed) {
 	printf("\n");
  }
  if (flush) fflush(stdout); 
  return SMACQ_PASS;
}

printModule::printModule(struct smacq_init * context) {
  struct state * state;
  smacq_opt verbose, flush, delimiter;
  int i;

  context->state = state = (struct state*) calloc(sizeof(struct state),1);
  assert(state);

  env = context->env;
  {
    struct smacq_optval optvals[] = {
      {"v", &verbose},
      {"d", &delimiter},
      {"B", &flush},
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &argc, &argv,
				 options, optvals);
    assert(argv);
  }

  flush = flush.boolean_t;
  delimiter = delimiter.string_t;
  verbose = verbose.boolean_t;
  fields = malloc(argc * sizeof(dts_field));
  string_transform = dts->requirefield("string");

  for (i = 0; i < argc; i++) {
	  fields[i] = dts->requirefield(dts_fieldname_append(argv[i],"string")); 
  }
  return 0;
}

printModule::~printModule(struct state * state) {
  int i;

  for (i = 0; i < argc; i++) 
	  dts_field_free(fields[i]);

  free(fields);
  return 0;
}




/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_test_dynamic_table = {
	produce: &print_produce, 
	consume: &print_consume,
	init: &print_init,
	shutdown: &print_shutdown,
};

