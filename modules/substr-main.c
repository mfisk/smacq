#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "smacq.h"
#include "substr/substr.h"

struct state {
  smacq_environment * env;
  int demux;
  struct smacq_outputq * outputq;
  struct batch * batch;
  int num_batches;
};

struct batch {
  struct ruleset * set;
  dts_field field;
  char * fieldname;
};

static struct smacq_options options[] = {
  {"f", {string_t:NULL}, "Field to inspect (full data is default)", SMACQ_OPT_TYPE_STRING},
  {"m", {boolean_t:0}, "OR multiple fields and demux to individual outputs", SMACQ_OPT_TYPE_BOOLEAN},
  {NULL, {NULL}, NULL, 0}
};

static int cdecode(unsigned char * needle) {
  int used;
  int i;
  int esc = 0;
  int len = strlen(needle);
  unsigned char * decoded = malloc(len+1);
  unsigned char * dp = decoded;
  unsigned char * np = needle;

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

static void add_entry(struct state * state, char * field, char * needle, int output) {
  struct batch * batch;
  int nlen;
  if (!field && !needle) return;

  if (!needle) {
  	needle = field;
	field = NULL;
  }

  if (field) {
	int i;
	int found = 0;
	dts_field f = smacq_requirefield(state->env, field);

	for (i = 0; i < state->num_batches; i++) {
		if (dts_comparefields(f, state->batch[i].field)) {
			batch = &state->batch[i];
			found = 1;
		}
	}

	if (!found) {
		state->num_batches++;
		state->batch = realloc(state->batch, state->num_batches * sizeof(struct batch));
		batch = &state->batch[state->num_batches-1];

    		batch->field = smacq_requirefield(state->env, field);
		batch->fieldname = field;
  		batch->set = substr_new(SUBSTR_FAST);
	}
  }

  nlen = strlen(needle);
#ifdef DEBUG
  fprintf(stderr, "decoded %s(%d) to search string of ", needle, nlen);
#endif
  nlen = cdecode(needle);
#ifdef DEBUG
  fprintf(stderr, "%s(%d)\n", needle, nlen);
#endif

  substr_add(batch->set, nlen, needle, 0, (void*)output, 0, 0);
}

static smacq_result substr_produce(struct state* state, const dts_object ** datum, int * outchan) {
  return smacq_produce_dequeue(&state->outputq, datum, outchan);
}

static smacq_result substr_consume(struct state * state, const dts_object * datum, int * outchan) {
  struct substr_search_result res;
  const dts_object * field;
  int matched = 0;
  int chan;
  int i;

  assert(datum);

  for (i=0; i < state->num_batches; i++) {
     struct batch * batch = &state->batch[i];
     if (batch->field) {
  	field = smacq_getfield(state->env, datum, batch->field, NULL);
  	if (!field) return SMACQ_FREE;
     } else {
        field = datum;
     }
  
     memset(&res, 0, sizeof(res));

     while(1) {
	  if (! substr_search(batch->set, field->data, field->len, &res)) 
		  break;

	  chan = (int)res.p->handle;

#ifdef DEBUG
	  fprintf(stderr, "pattern match '%.*s' at offset %d output %d\n", res.p->len, res.p->pattern, res.shift, chan);
#endif

	  if (matched) {
		smacq_produce_enqueue(&state->outputq, datum, chan);
		dts_incref(datum, 1);
	  } else {
	  	matched = 1;
		*outchan = chan;
	  } 
      }

      if (batch->field) 
	  dts_decref(field);

  }

  if (matched) return SMACQ_PASS;

  return SMACQ_FREE;
}

static smacq_result substr_init(struct smacq_init * context) {
  struct state * state;
  int i, argc;
  char ** argv;
  smacq_opt field_opt, demux;
  char * field = NULL;
  char * pattern = NULL;
  int output = 0;

  context->state = state = (struct state*) calloc(sizeof(struct state),1);
  assert(state);

  state->batch = calloc(sizeof(struct batch), 1);

  state->env = context->env;
  {
    struct smacq_optval optvals[] = {
      {"f", &field_opt},
      {"m", &demux},
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &argc, &argv,
				 options, optvals);

    state->demux = demux.boolean_t;
  }

  for (i = 0; i < argc; i++) {
	  if (!strcmp(argv[i], ";") && pattern) {
		if (!field) field = field_opt.string_t; 
		//fprintf(stderr, "middle pattern is %s\n", pattern);
		add_entry(state, field, pattern, output++);
		field = NULL;
		pattern = NULL;
	  } else if (!field) {
		field = argv[i];
	  } else {
		pattern = argv[i];
	  }
  }
  if (pattern) {
	if (!field) field = field_opt.string_t; 
	//fprintf(stderr, "last pattern of %p is %s\n", context->self, pattern);
  	add_entry(state, field, pattern, output++);
  }

  if (output>1) {
	  state->demux = 1;
  }

  for (i=0; i < state->num_batches; i++) {
  	substr_compile(state->batch[i].set);
  }
  return 0;
}

static smacq_result substr_shutdown(struct state * state) {
  free(state);
  return 0;
}

struct smacq_functions smacq_substr_table = {
	produce: &substr_produce, 
	consume: &substr_consume,
	init: &substr_init,
	shutdown: &substr_shutdown,
	algebra: {vector: 1, boolean: 1}
};

