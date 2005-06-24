#ifdef linux
#define _FILE_OFFSET_BITS 64
#define _LARGEFILE64_SOURCE 1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <SmacqModule.h>

static struct smacq_options options[] = {
  {"d", {string_t:"\t"}, "Delimiter", SMACQ_OPT_TYPE_STRING},
  {"f", {string_t:"-"}, "Input File", SMACQ_OPT_TYPE_STRING},
  END_SMACQ_OPTIONS
};

SMACQ_MODULE(tabularinput,
  PROTO_CTOR(tabularinput);
  PROTO_PRODUCE();
  
  char  delimiter;
  FILE * fh;

  DynamicArray<DtsField> fieldvec;
  int * field_type;
  int fields;

  dts_typeid string_type;
  dts_typeid double_type;
  dts_typeid empty_type;

  DtsObject default_parse(char * startp, char * endp);
);

#define MAX_STR 4096

DtsObject tabularinputModule::default_parse(char * startp, char * endp) {
      char * badp;
      DtsObject msgdata;
      double d = strtod(startp, &badp);

      if (badp && badp != endp) {
	//fprintf(stderr, "Double test failed, '%s' remains\n", badp);
	msgdata = dts->construct_fromstring(string_type, startp);
      } else {
	//fprintf(stderr, "Double test succeeded for '%s'\n", startp);
	msgdata = dts->construct(double_type, &d);
      }

      return msgdata;
}

smacq_result tabularinputModule::produce(DtsObject & datump, int & outchan) {
  int i;
  char * startp, * stopp, * endp, line[MAX_STR];
  DtsObject msgdata;
  char * result;
  DtsObject datum;
  DtsField field;

  datum = dts->newObject(empty_type);
  
  result = fgets(line, MAX_STR, fh);
  if (!result) {
    return SMACQ_END;
  }
  stopp = index(line, '\n');
  if (!stopp) {
    if (strlen(line) < MAX_STR) {
    	//fprintf(stderr, "tabularinput: Warning: Unterminated line: %s\n", line);
    	stopp = line+MAX_STR-1;
    } else {
	fprintf(stderr, "tabularinput: Error: exceeded maximum line length of %d\n", MAX_STR);
	assert(0);
    }
  }

  stopp[0] = '\0';

  for (i = 0, startp=line, stopp=line+strlen(line), endp=NULL; 
       endp < stopp; 
       i++, startp=endp+1) {

    endp = index(startp, delimiter);
    if (!endp) {
      endp = stopp;
    } else {
      endp[0] = '\0';
    }
 
    if (i >= fields || !field_type[i]) {
	msgdata = default_parse(startp, endp);
    } else {
	msgdata = dts->construct_fromstring(field_type[i], startp);
	if (!msgdata) {
	    fprintf(stderr, "Unable to parse field %s as type %s\n", "", "");
	    msgdata = default_parse(startp, endp);
	}
    }

    assert(msgdata);

    field = fieldvec[i];
    if (! field) {
      char buf[1024];
      sprintf(buf, "%d", i+1);
      field = dts->requirefield(buf);
      fieldvec[i] = field;
    }
    datum->attach_field(field, msgdata); 
    //fprintf(stderr, "Attached field %d (type %d) to %p\n", field[0], msgdata->type, datum);
  }

  datump = datum;
  return (smacq_result)(SMACQ_PASS|SMACQ_PRODUCE);
}

tabularinputModule::tabularinputModule(struct SmacqModule::smacq_init * context) : SmacqModule(context) {
  smacq_opt delimitero, fileo;
  int argc;
  char ** argv;
  int i;

  {
    struct smacq_optval optvals[] = {
      { "d", &delimitero}, 
      { "f", &fileo}, 
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &argc, &argv,
				 options, optvals);
  }

  delimiter = delimitero.string_t[0];
  if (!strcmp(fileo.string_t,"-")) {
	  fh = stdin;
  } else {
  	fh = fopen(fileo.string_t, "r");
  	if (!fh) {
	  	perror("Unable to open file\n");
		assert(0);
  	}
  }

  fields = argc;
  field_type = (int*)calloc(argc, sizeof(int));
  
  for (i = 0; i < argc; i++) {
    char * name = strdup(argv[i]);
    char * type;
    type = index(name, ':');

    if (!type) {
      field_type[i] = 0;
    } else {
      type[0] = '\0';
      field_type[i] = dts->requiretype(type+1);
    }
    fieldvec[i] = dts->requirefield(name);
    free(name);
  }
  
  if (!context->isfirst) {
    fprintf(stderr, "tabularinput is a data source only.\n");
    assert(0);
  }

  double_type = dts->requiretype("double");
  string_type = dts->requiretype("string");
  empty_type = dts->requiretype("empty");
}

