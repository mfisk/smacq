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
	     FILE * outputfh;
	     bool verbose, tagged, flush, internals, boulder, binary, use_file_pattern, mysql;
	     std::vector<DtsField> fields;
	     DtsField string_transform, filefield;
	     char * delimiter, * record_delimiter, * output_name;
	     
	     int print_field(DtsObject field, char * fname, int printed, int column);
	     char * last_file_field;

	     );

static struct smacq_options options[] = {
  {"x", {boolean_t:0}, "XML-markup mode", SMACQ_OPT_TYPE_BOOLEAN},
  {"v", {boolean_t:0}, "Verbose mode: print field names", SMACQ_OPT_TYPE_BOOLEAN},
  {"i", {boolean_t:0}, "Print internal debugging info", SMACQ_OPT_TYPE_BOOLEAN},
  {"d", {string_t:"\t"}, "Delimiter", SMACQ_OPT_TYPE_STRING},
  {"r", {string_t:"\n"}, "Delimiter between records", SMACQ_OPT_TYPE_STRING},
  {"boulder", {boolean_t:0}, "Print records in Boulder format", SMACQ_OPT_TYPE_BOOLEAN},
  {"binary", {boolean_t:0}, "Binary output", SMACQ_OPT_TYPE_BOOLEAN},
  {"mysql", {boolean_t:0}, "MySQL output", SMACQ_OPT_TYPE_BOOLEAN},
  {"B", {boolean_t:0}, "Disable buffering: flush output after each line", SMACQ_OPT_TYPE_BOOLEAN},
  {"f", {string_t:"-"}, "Output file", SMACQ_OPT_TYPE_STRING},
  {"filefield", {string_t:NULL}, "Field substituted into file name pattern", SMACQ_OPT_TYPE_STRING},
  END_SMACQ_OPTIONS
};

int printModule::print_field(DtsObject field, char * fname, int printed, int column) {
  if (printed) {
    fprintf(outputfh, delimiter);
  } else if (field) {
    for (int j=0; j<column; j++) 
      fprintf(outputfh, delimiter);
  }

  if (verbose) printed = 1;
  if (!field) return printed;

  if (tagged) {
    fprintf(outputfh, "<%s>%s</%s>", fname, (char *)field->getdata(),fname);
  } else if (boulder) {
    fprintf(outputfh, "%s=%s", fname, (char *)field->getdata());
  } else if (verbose) {
    fprintf(outputfh, "%.20s = %s", fname, (char *)field->getdata());
  } else {
    fprintf(outputfh, "%s", (char*)field->getdata());
  }
        

  return 1;
}

smacq_result printModule::consume(DtsObject datum, int & outchan) {
  int printed = 0;
  int column = 0;
  DtsObject field;
  assert(datum);

  if (use_file_pattern) {
    char buf[4096];
    DtsObject ff = datum->getfield(filefield);
    if (!ff) {
	return SMACQ_PASS;
    }
    char * this_file_field = (char*)ff->getdata();
    if (!outputfh || strcmp(last_file_field, this_file_field)) {
	last_file_field = this_file_field;
    	snprintf(buf, 4095, output_name, (char*)ff->getdata());

	// MySQL doesn't like periods
	if (mysql) {
		char * i = buf; 
		while ((i=index(i, '.'))) {
			*i = '_';
		}
		strncat(buf, ".MYD", 4095);
	}
	
    	if (outputfh) fclose(outputfh);  
    	outputfh = fopen(buf, "a");
    	if (!outputfh) {
		perror("open");
		return SMACQ_PASS;
    	}
    }
  }
  if (!fields.size()) {
    /* Print all fields */

    if (internals) {
      fprintf(outputfh, "Object %p:\n", datum.get());
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
	  char * s = "(unprintable)";
	  if (str.get()) { 
	    s = (char*)str->getdata();
	  }
	  fprintf(outputfh, "\tField %2d: %15s = (obj %p) %s\n", num, 
		  dts->field_getname(f), j->get(), s);
	} else {
	  printed = print_field(str, dts->field_getname(f), printed, column);
	}
      }
    }
      
  } else {
    for (int i = 0; i < argc; i++) {
      field = datum->getfield(fields[i]);
      if (binary) {
	if (mysql) 
	  fwrite("\001\000\000", 3, 1, outputfh);

	if (field) 
	  fwrite(field->getdata(), field->getsize(), 1, outputfh);
      } else { 
	printed = print_field(field, argv[i], printed, column);
	
	if (!field && verbose) {
	  if (tagged) {
	    fprintf(outputfh, "no field %s.string", argv[i]);
	  } else {
	    fprintf(stderr, "Warning: print: no field %s.string\n", argv[i]);
	  }
	}
	column++;
      }
    }
  }
  if (printed) {
    fprintf(outputfh, record_delimiter);
  }
  if (flush) {
    fflush(stdout); 
  }
  return SMACQ_PASS;
}

printModule::printModule(struct smacq_init * context) 
 : SmacqModule(context), use_file_pattern(false) {
  smacq_opt boulder_opt, record_delimiter_opt, tagged_opt, verbose_opt, 
    flush_opt, delimiter_opt, internals_opt, output_opt, binary_opt,
    mysql_opt, filefield_opt;
  int i;
  
  {
    struct smacq_optval optvals[] = {
      {"x", &tagged_opt},
      {"v", &verbose_opt},
      {"r", &record_delimiter_opt},
      {"d", &delimiter_opt},
      {"B", &flush_opt},
      {"f", &output_opt},
      {"filefield", &filefield_opt},
      {"boulder", &boulder_opt},
      {"binary", &binary_opt},
      {"mysql", &mysql_opt},
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
  record_delimiter = record_delimiter_opt.string_t;
  verbose = verbose_opt.boolean_t;
  boulder = boulder_opt.boolean_t;
  binary = binary_opt.boolean_t;
  tagged = tagged_opt.boolean_t;

  mysql = mysql_opt.boolean_t;
  if (mysql) binary = true;

  if (filefield_opt.string_t) {
    filefield = dts->requirefield(dts_fieldname_append(filefield_opt.string_t, "string"));
    use_file_pattern = true;
    outputfh = NULL;
    output_name = output_opt.string_t;
    // defer until we have data
  } else if (strcmp(output_opt.string_t, "-")) {
    outputfh = fopen(output_opt.string_t, "w");
    if (!outputfh) {
      fprintf(stderr, "print: error writing to %s\n", output_opt.string_t);
      exit(-1);
    }
  } else {
    outputfh = stdout;
  }
  if (boulder) {
    delimiter = "\n";
    record_delimiter = "\n=\n";
  }

  fields.resize(argc);

  //fprintf(stderr, "creating 1.75 print module, argc=%d, dts=%p\n", argc, dts);
  string_transform = dts->requirefield("string");

  //fprintf(stderr, "creating2 print module\n");

  if (binary) {
    for (i = 0; i < argc; i++) {
      fields[i] = dts->requirefield(argv[i]); 
    }
  } else {
    for (i = 0; i < argc; i++) {
      fields[i] = dts->requirefield(dts_fieldname_append(argv[i],"string")); 
    }
  }
}

