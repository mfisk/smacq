#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "substr/substr.h"

#define SMACQ_MODULE_IS_VECTOR 1
#define SMACQ_MODULE_IS_STATELESS 1
#include <SmacqModule.h>

SMACQ_MODULE(substr,
  PROTO_CTOR(substr);
  PROTO_CONSUME();

  int demux;
  struct batch * batch;
  int num_batches;

  void add_entry(const char * field, const char * needle, int output);
);

struct batch {
  struct ruleset * set;
  DtsField field;
  const char * fieldname;
};

static struct smacq_options options[] = {
  {"f", {string_t:NULL}, "Field to inspect (full data is default)", SMACQ_OPT_TYPE_STRING},
  {"m", {boolean_t:0}, "OR multiple fields and demux to individual outputs", SMACQ_OPT_TYPE_BOOLEAN},
  END_SMACQ_OPTIONS
};

// In-place decode
static int cdecode(char * needle) {
  int used;
  int esc = 0;
  int len = strlen((char*)needle);
  char * decoded = (char*)malloc(len+1);
  char * dp = decoded;
  char * np = needle;

  while (np < needle + len) {
	  if (esc) {
		esc = 0;
		if (*np == 'x') {
			np[-1] = '0'; /* make 0x... */
			sscanf(np-1, "%4hhi%n", dp++, &used);
			//fprintf(stderr, "\n hex decoded %d chars of %s to val %i\n", used, np-1, dp[-1]);
			np += used - 1;
		} else if ((*np == 'o') || (*np == '0')) {
			np[0] = '0'; /* make 0... */
			sscanf(np, "%4hhi%n", dp++, &used);
			//fprintf(stderr, "\n oct decoded %d chars of %s to val %i\n", used, np-1, dp[-1]);
			np += used;
		} else {
			//fprintf(stderr, "\ngot escaped `%c'\n", *np);
			*dp++ = *np++;
		}
		continue;

	  } else if (*np == '\\') {
		esc = 1;
		np++;
          } else {
	  	*dp++ = *np++;
	  }
  }

  /* return length is the actual length (without null terminator) */
  len = dp - decoded;

  memmove(needle, decoded, len);
  needle[len] = '\0'; /* In case somebody wants to print it */
  free(decoded);
  return len;
}

void substrModule::add_entry(const char * field, const char * needle, int output) {
  struct batch * mybatch = NULL;
  int nlen;
  if (!field && !needle) return;

  if (!needle) {
  	needle = (char*)field;
	field = NULL;
  }

  if (field) {
	int i;
	int found = 0;
	DtsField f = usesfield(field);

	for (i = 0; i < num_batches; i++) {
		if (dts_comparefields(f, batch[i].field)) {
			mybatch = &batch[i];
			found = 1;
		}
	}

	if (!found) {
		num_batches++;
		batch = (struct batch*)realloc(batch, num_batches * sizeof(struct batch));
		mybatch = &batch[num_batches-1];

    		mybatch->field = usesfield(field);
		mybatch->fieldname = field;
  		mybatch->set = substr_new(SUBSTR_FAST);
	}
  }

  nlen = strlen(needle);
#ifdef DEBUG
  fprintf(stderr, "decoded %s(%d) to search string of ", needle, nlen);
#endif
  nlen = cdecode((char*)needle);
#ifdef DEBUG
  fprintf(stderr, "%s(%d)\n", needle, nlen);
#endif

  substr_add(mybatch->set, nlen, (unsigned char*)needle, 0, (void*)(intptr_t)output, 0, 0);
}

smacq_result substrModule::consume(DtsObject datum, int & outchan) {
  struct substr_search_result res;
  DtsObject field;
  int matched = 0;
  long int chan;
  int i;

  assert(datum);

  for (i=0; i < num_batches; i++) {
     struct batch * mybatch = &batch[i];
     if (mybatch->field) {
  	field = datum->getfield(mybatch->field);
  	if (!field) return SMACQ_FREE;
     } else {
        field = datum;
     }
  
     memset(&res, 0, sizeof(res));

     while(1) {
	  if (! substr_search(mybatch->set, field->getdata(), field->getsize(), &res)) 
		  break;

	  chan = (long int)res.p->handle;

#ifdef DEBUG
	  fprintf(stderr, "pattern match '%.*s' at offset %d output %d\n", res.p->len, res.p->pattern, res.shift, chan);
#endif

	  if (matched) {
		enqueue(datum, chan);
		
	  } else {
	  	matched = 1;
		outchan = chan;
	  } 
      }
  }

  if (matched) {
	return SMACQ_PASS;
  } else {
  	return SMACQ_FREE;
  }

}

substrModule::substrModule(struct SmacqModule::smacq_init * context) : SmacqModule(context), num_batches(0) {
  int i, argc;
  const char ** argv;
  smacq_opt field_opt, demux_opt;
  const char * field = NULL;
  const char * pattern = NULL;
  int output = 0;

  batch = (struct batch*)calloc(sizeof(struct batch), 1);

  {
    struct smacq_optval optvals[] = {
      {"f", &field_opt},
      {"m", &demux_opt},
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &argc, &argv,
				 options, optvals);

    demux = demux_opt.boolean_t;
  }

  for (i = 0; i < argc; i++) {
	  if (!strcmp(argv[i], ";") && pattern) {
		if (!field) field = field_opt.string_t; 
		//fprintf(stderr, "middle pattern is %s\n", pattern);
		add_entry(field, pattern, output++);
		field = NULL;
		pattern = NULL;
	  } else if (!field) {
		field = argv[i];
	  } else {
		pattern = (char*)argv[i];
	  }
  }
  if (pattern) {
	if (!field) field = field_opt.string_t; 
	//fprintf(stderr, "last pattern of %p is %s\n", context->self, pattern);
  	add_entry(field, pattern, output++);
  }

  if (output>1) {
	  demux = 1;
  }

  for (i=0; i < num_batches; i++) {
  	substr_compile(batch[i].set);
  }
}

