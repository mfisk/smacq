#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "smacq.h"
#include "substr/substr.h"

struct state {
  smacq_environment * env;
  dts_field field;
  struct ruleset * set;
  int demux;
  char * fieldname;
  struct smacq_outputq * outputq;
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

  for (i=0; i < len; i++) {
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
  int nlen;
  if (!field && !needle) return;

  if (!needle) {
  	needle = field;
	field = NULL;
  }

  if (field) {
	if (!state->fieldname) {
    		state->field = smacq_requirefield(state->env, field);
		state->fieldname = field;
	} else if (strcmp(state->fieldname, field)) {
		/* XXX: Uh-oh, we don't know how to search multiple fields */
		assert(!"Cannot mix field names");
	}
  }

#ifdef DEBUG
  fprintf(stderr, "searching for '%s'\n", needle);
#endif
  nlen = strlen(needle);

  //fprintf(stderr, "decoded %s(%d) to ", needle, nlen);
  nlen = cdecode(needle);
  //fprintf(stderr, "%s(%d)\n", needle, nlen);

  substr_add(state->set, nlen, needle, 0, (void*)output, 0, 0);
}

static smacq_result substr_produce(struct state* state, const dts_object ** datum, int * outchan) {
  return smacq_produce_dequeue(&state->outputq, datum, outchan);
}

static smacq_result substr_consume(struct state * state, const dts_object * datum, int * outchan) {
  struct substr_search_result res;
  const dts_object * field;
  int matched = 0;
  int chan;

  assert(datum);

  if (state->field) {
  	field = smacq_getfield(state->env, datum, state->field, NULL);
  	if (!field) return SMACQ_FREE;
  } else {
        field = datum;
  }
  
  memset(&res, 0, sizeof(res));

  while(1) {
	  if (! substr_search(state->set, field->data, field->len, &res)) 
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

  if (state->field) 
	  dts_decref(field);

  if (matched) return SMACQ_PASS;

  return SMACQ_FREE;
}

static int substr_init(struct smacq_init * context) {
  struct state * state;
  int i, argc;
  char ** argv;
  smacq_opt field, demux;
  char * a = NULL;
  char * b = NULL;
  int output = 0;

  context->state = state = (struct state*) calloc(sizeof(struct state),1);
  assert(state);

  state->env = context->env;
  {
    struct smacq_optval optvals[] = {
      {"f", &field},
      {"m", &demux},
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &argc, &argv,
				 options, optvals);

    state->field = smacq_requirefield(state->env, field.string_t);
    state->demux = demux.boolean_t;
  }

  state->set = substr_new(SUBSTR_FAST);

  for (i = 0; i < argc; i++) {
	  if (!strcmp(argv[i], ";")) {
		add_entry(state, a, b, output++);
		a = NULL;
		b = NULL;
	  } else if (!a) {
		a = argv[i];
	  } else {
		b = argv[i];
	  }
  }
  add_entry(state, a, b, output++);

  if (output>1) {
	  state->demux = 1;
  }

  substr_compile(state->set);
  return 0;
}

static int substr_shutdown(struct state * state) {
  free(state);
  return 0;
}

struct smacq_functions smacq_substr_table = {
	produce: &substr_produce, 
	consume: &substr_consume,
	init: &substr_init,
	shutdown: &substr_shutdown,
	alg: {vector: 1, boolean: 1}
};

