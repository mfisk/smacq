#ifdef linux
#define _FILE_OFFSET_BITS 64
#define _LARGEFILE64_SOURCE 1
#endif
#define __STDC_LIMIT_MACROS
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <dts.h>
#include <SmacqModule.h>
#include <stdint.h>
#include <math.h>

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
	     
	     void print_field(DtsObject fieldo, char * fname);

	     char * last_file_field;

	     // Per-record state
	     int printed, column;

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

void printModule::print_field(DtsObject field, char * fname) {
  if (printed) {
    fprintf(outputfh, delimiter);
  } else if (field) {
    for (int j=0; j<column; j++) 
      fprintf(outputfh, delimiter);
  }

  if (verbose) printed = 1;
  if (!field) return; 

  if (tagged) {
    fprintf(outputfh, "<%s>%s</%s>", fname, (char *)field->getdata(),fname);
  } else if (boulder) {
    fprintf(outputfh, "%s=%s", fname, (char *)field->getdata());
  } else if (verbose) {
    fprintf(outputfh, "%.20s = %s", fname, (char *)field->getdata());
  } else if (internals) {
    fprintf(outputfh, "%.20s = (obj %p) %s", fname, field.get(), (char *)field->getdata());
  } else {
    fprintf(outputfh, "%s", (char*)field->getdata());
  }
        

  printed = 1;
}

smacq_result printModule::consume(DtsObject datum, int & outchan) {
  printed = 0;
  column = 0;
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

    datum->prime_all_fields();
    std::vector<DtsObject> fc = datum->fieldcache();
    for (unsigned int i = 0; i < fc.size(); ++i) {
	if (fc[i]) {
  		DtsField f(i);
  		DtsObject fo = datum->getfield(f);
  		fo = fo->getfield(string_transform);
  		print_field(fo, dts->field_getname(f));
	}
    }
  } else {
    DtsObject field;
    if (mysql) {
	  // Write record header
	  //
	  // The MyISAM record header is described in:
	  // 	 http://dev.mysql.com/doc/internals/en/myisam-introduction.html
	  // Iff there are NULL columns, it is a bitmask for each column plus a least 
	  // significant bit describing whether the row is deleted.  Iff there are no 
	  // NULL columns, the header is truncated to the least significant byte
	  
	  // This does the right thing for a non-truncated header:
	  //int numbytes = lround(ceil((1+argc)/8.0));
	  
	  // This does the right thing for a truncated header:
	  int numbytes = 1;  // Since we don't support NULL colums

	  for (int b = numbytes; b > 0; b--) {
		  unsigned int val = 0;  //Need a 0 for each non-NULL column
		  if (b == numbytes) {
			  int nonpadbits = ((1+argc) % 8);
			  //fprintf(stderr, "nonpadbits is %d for %d args\n", nonpadbits, argc);
			  val = ~0;
			  val <<= nonpadbits;  //Need a 0 for each non-NULL column
		  }
		  if (b == 1) {
			  val |= 1;  //LSB is 1 for not-deleted record
		  }
	  	  fwrite(&val, 1, 1, outputfh);
	  }
    }
    for (int i = 0; i < argc; i++) {
      field = datum->getfield(fields[i]);
      if (binary) {
	if (field) 
	  fwrite(field->getdata(), field->getsize(), 1, outputfh);
      } else { 
	print_field(field, argv[i]);
	
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

