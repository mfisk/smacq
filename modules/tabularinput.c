#ifdef linux
#define _FILE_OFFSET_BITS 64
#define _LARGEFILE64_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <smacq.h>

static struct smacq_options options[] = {
  {"d", {string_t:"\t"}, "Delimiter", SMACQ_OPT_TYPE_STRING},
  {"f", {string_t:"-"}, "Input File", SMACQ_OPT_TYPE_STRING},
  {NULL, {string_t:NULL}, NULL, 0}
};

struct state {
  smacq_environment * env;
  char  delimiter;
  FILE * fh;

  struct darray field_name;
  int * field_type;
  int fields;

  int string_type;
  int double_type;
  int empty_type;
};

#define MAX_STR 4096

static const dts_object * default_parse(struct state* state, char * startp, char * endp) {
      char * badp;
      const dts_object * msgdata;
      double d = strtod(startp, &badp);

      if (badp && badp != endp) {
	//fprintf(stderr, "Double test failed, '%s' remains\n", badp);
	msgdata = smacq_construct_fromstring(state->env, state->string_type, startp);
      } else {
	//fprintf(stderr, "Double test succeeded for '%s'\n", startp);
	msgdata = smacq_dts_construct(state->env, state->double_type, &d);
      }

      return msgdata;
}

static smacq_result tabularinput_produce(struct state* state, const dts_object ** datump, int * outchan) {
  int i;
  char * startp, * stopp, * endp, line[MAX_STR];
  const dts_object * msgdata;
  char * result;
  const dts_object * datum;
  dts_field field;
  assert(state);

  datum = smacq_alloc(state->env, 0, state->empty_type);
  
  result = fgets(line, MAX_STR, state->fh);
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

    endp = index(startp, state->delimiter);
    if (!endp) {
      endp = stopp;
    } else {
      endp[0] = '\0';
    }
 
    if (i >= state->fields || !state->field_type[i]) {
	msgdata = default_parse(state, startp, endp);
    } else {
	msgdata = smacq_construct_fromstring(state->env, state->field_type[i], startp);
	if (!msgdata) {
	    fprintf(stderr, "Unable to parse field %s as type %s\n", "", "");
	    msgdata = default_parse(state, startp, endp);
	}
    }

    assert(msgdata);

    field = darray_get(&state->field_name, i);
    if (! field) {
      char buf[1024];
      sprintf(buf, "%d", i+1);
      field = smacq_requirefield(state->env, buf);
      darray_set(&state->field_name, i, field);
    }
    dts_attach_field(datum, field, msgdata); 
    //fprintf(stderr, "Attached field %d (type %d) to %p\n", field[0], msgdata->type, datum);
  }

  *datump = datum;
  return(SMACQ_PASS|SMACQ_PRODUCE);
}

static smacq_result tabularinput_consume(struct state * state, const dts_object * datum, int * outchan) {
  assert(0);

  return SMACQ_FREE;
}


static smacq_result tabularinput_shutdown(struct state * state) {
  return 0;
}

static smacq_result tabularinput_init(struct smacq_init * context) {
  struct state * state;
  smacq_opt delimitero, fileo;
  int argc;
  char ** argv;
  int i;

  context->state = state = (struct state*) calloc(sizeof(struct state),1);
  assert(state);
  state->env = context->env;

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

  state->delimiter = delimitero.string_t[0];
  if (!strcmp(fileo.string_t,"-")) {
	  state->fh = stdin;
  } else {
  	state->fh = fopen(fileo.string_t, "r");
  	if (!state->fh) {
	  	perror("Unable to open file\n");
	  	return(SMACQ_ERROR);
  	}
  }

  state->fields = argc;
  darray_init(&state->field_name, argc);
  state->field_type = calloc(argc, sizeof(int));
  
  for (i = 0; i < argc; i++) {
    char * name = strdup(argv[i]);
    char * type;
    type = index(name, ':');

    if (!type) {
      state->field_type[i] = 0;
    } else {
      type[0] = '\0';
      state->field_type[i] = smacq_requiretype(state->env, type+1);
    }
    darray_set(&state->field_name, i, smacq_requirefield(state->env, name));
    free(name);
  }
  
  if (!context->isfirst) {
    fprintf(stderr, "tabularinput is a data source only.\n");
    assert(0);
  }

  state->double_type = smacq_requiretype(state->env, "double");
  state->string_type = smacq_requiretype(state->env, "string");
  state->empty_type = smacq_requiretype(state->env, "empty");

  return 0;
}


/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_tabularinput_table = {
	produce: &tabularinput_produce, 
	consume: &tabularinput_consume,
	init: &tabularinput_init,
	shutdown: &tabularinput_shutdown
};

