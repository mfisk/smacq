#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <dts.h>

SMACQ_MODULE(print, 
	PROTO_CTOR(print);
	PROTO_DTOR(print);
	PROTO_CONSUME();

	     char ** argv;
	     int argc;
	     int verbose;
	     int tagged;
	     int flush;
	     dts_field * fields;
	     dts_field string_transform;
	     char * delimiter;
	     
	     int print_field(DtsObject field, char * fname, int printed, int column);
	     );

static struct smacq_options options[] = {
  {"x", {boolean_t:0}, "XML-markup mode", SMACQ_OPT_TYPE_BOOLEAN},
  {"v", {boolean_t:0}, "Verbose mode: print field names", SMACQ_OPT_TYPE_BOOLEAN},
  {"d", {string_t:"\t"}, "Delimiter", SMACQ_OPT_TYPE_STRING},
  {"B", {boolean_t:0}, "Disable buffering: flush output after each line", SMACQ_OPT_TYPE_BOOLEAN},
  END_SMACQ_OPTIONS
};

int printModule::print_field(DtsObject field, char * fname, int printed, int column) {
        if (printed) {
      	   printf(delimiter);
    	} else if (field) {
	   int j;
    	   for (j=0; j<column; j++) 
    		printf(delimiter);
        }

	if (verbose) printed = 1;
	if (!field) return printed;

	if (tagged) {
		printf("<%s>%s</%s>", fname, (char *)field->getdata(),fname);
	} else if (verbose) {
		printf("%.20s = %s", fname, (char *)field->getdata());
	} else {
		printf("%s", (char*)field->getdata());
	}
        

	return 1;
}

smacq_result printModule::consume(DtsObject datum, int * outchan) {
  int i;
  int printed = 0;
  int column = 0;
  DtsObject field;
  assert(datum);

  for (i = 0; i < argc; i++) {
    if (!fields[i]) {
      /* Print all fields */

      datum->prime_all_fields();
/*
  int j;
	for (j = 0; j <= datum->fields.max; j++) {
		field = datum->getfield_single(j);
		if (field) {
			printed = print_field(field, dts->fields_bynum(j), 
					      printed, column);
			column++;
		}
	}
	column--;
*/
    } else {
	field = datum->getfield(fields[i]);
	printed = print_field(field, argv[i], printed, column);

        if (!field && verbose) {
		if (tagged) {
			printf("no field %s.string", argv[i]);
		} else {
        		fprintf(stderr, "Warning: print: no field %s.string\n", argv[i]);
		}
	}
    }
    column++;
  }
  if (printed) {
 	printf("\n");
  }
  if (flush) fflush(stdout); 
  return SMACQ_PASS;
}

printModule::printModule(struct smacq_init * context) : SmacqModule(context) {
  smacq_opt tagged_opt, verbose_opt, flush_opt, delimiter_opt;
  int i;

  {
    struct smacq_optval optvals[] = {
      {"x", &tagged_opt},
      {"v", &verbose_opt},
      {"d", &delimiter_opt},
      {"B", &flush_opt},
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &argc, &argv,
				 options, optvals);
    assert(argv);
  }

  flush = flush_opt.boolean_t;
  delimiter = delimiter_opt.string_t;
  verbose = verbose_opt.boolean_t;
  tagged = tagged_opt.boolean_t;
  fields = (dts_field_element**)malloc(argc * sizeof(dts_field));

  //fprintf(stderr, "creating 1.75 print module, argc=%d, dts=%p\n", argc, dts);
  string_transform = dts->requirefield("string");

  //fprintf(stderr, "creating2 print module\n");

  for (i = 0; i < argc; i++) {
	  if (!strcmp(argv[i], "*")) {
		fields[i] = NULL;
	  } else {
	  	fields[i] = dts->requirefield(dts_fieldname_append(argv[i],"string")); 
	  }
  }
}

printModule::~printModule() {
  int i;

  for (i = 0; i < argc; i++) 
	  dts_field_free(fields[i]);

  free(fields);
}
