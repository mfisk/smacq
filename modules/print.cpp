#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <dts.h>
#include <SmacqModule.h>

SMACQ_MODULE(print, 
	PROTO_CTOR(print);
	PROTO_CONSUME();

   	char ** argv;
   	int argc;
	bool verbose;
	bool tagged;
	bool flush;
	bool internals;
   	std::vector<DtsField> fields;
	DtsField string_transform;
	char * delimiter;
	
	int print_field(DtsObject field, char * fname, int printed, int column);
);

static struct smacq_options options[] = {
  {"x", {boolean_t:0}, "XML-markup mode", SMACQ_OPT_TYPE_BOOLEAN},
  {"v", {boolean_t:0}, "Verbose mode: print field names", SMACQ_OPT_TYPE_BOOLEAN},
  {"i", {boolean_t:0}, "Print internal debugging info", SMACQ_OPT_TYPE_BOOLEAN},
  {"d", {string_t:"\t"}, "Delimiter", SMACQ_OPT_TYPE_STRING},
  {"B", {boolean_t:0}, "Disable buffering: flush output after each line", SMACQ_OPT_TYPE_BOOLEAN},
  END_SMACQ_OPTIONS
};

int printModule::print_field(DtsObject field, char * fname, int printed, int column) {
	if (printed) {
		printf(delimiter);
   	} else if (field) {
		for (int j=0; j<column; j++) 
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

smacq_result printModule::consume(DtsObject datum, int & outchan) {
  int i;
  int printed = 0;
  int column = 0;
  DtsObject field;
  assert(datum);

  if (!fields.size()) {
      /* Print all fields */

	  if (internals) {
		printf("Object %p:\n", datum.get());
  	  }

      DtsField f;
      f.resize(1);
      datum->prime_all_fields();
      std::vector<DtsObject> v = datum->get_all_fields();
      std::vector<DtsObject>::iterator j;

	  int num;
      for (num=0, j=v.begin(); j != v.end(); ++j, ++num) {
			if (*j) {
				DtsObject str = (*j)->getfield(string_transform);
				f[0] = num;
				if (internals) {
					printf("\tField %2d: %15s = (obj %p) %s\n", num, 
						dts->field_getname(f), j->get(), (char*)str->getdata());
				} else {
					printed = print_field(str, dts->field_getname(f), printed, column);
				}
			}
      }

  } else {
	for (i = 0; i < argc; i++) {
			field = datum->getfield(fields[i]);
			printed = print_field(field, argv[i], printed, column);

			if (!field && verbose) {
				if (tagged) {
					printf("no field %s.string", argv[i]);
				} else {
       				fprintf(stderr, "Warning: print: no field %s.string\n", argv[i]);
				}
			}
    		column++;
	}
  }
  if (printed) {
 		printf("\n");
  }
  if (flush) {
		fflush(stdout); 
	}
  return SMACQ_PASS;
}

printModule::printModule(struct smacq_init * context) : SmacqModule(context) {
  smacq_opt tagged_opt, verbose_opt, flush_opt, delimiter_opt, internals_opt;
  int i;

  {
    struct smacq_optval optvals[] = {
      {"x", &tagged_opt},
      {"v", &verbose_opt},
      {"d", &delimiter_opt},
      {"B", &flush_opt},
      {"i", &internals_opt},
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &argc, &argv,
				 options, optvals);
    //assert(argv);
  }

  internals = internals_opt.boolean_t;
  flush = flush_opt.boolean_t;
  delimiter = delimiter_opt.string_t;
  verbose = verbose_opt.boolean_t;
  tagged = tagged_opt.boolean_t;
  fields.resize(argc);

  //fprintf(stderr, "creating 1.75 print module, argc=%d, dts=%p\n", argc, dts);
  string_transform = dts->requirefield("string");

  //fprintf(stderr, "creating2 print module\n");

  for (i = 0; i < argc; i++) {
	  fields[i] = dts->requirefield(dts_fieldname_append(argv[i],"string")); 
  }
}

