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

  int * field_name;
  int * field_type;
  int fields;

  int string_type;
  int double_type;
  int empty_type;
};

static inline const dts_object * flow_construct_fromstring(smacq_environment * env, int type, void * data) {
  const dts_object * o = flow_alloc(env, 0, type);
  dts_incref(o, 1);
  if (flow_fromstring(env, type, data, (dts_object*)o)) {
    return o;
  } else {
    dts_decref(o);
    return NULL;
  }
}

#define MAX_STR 4096

static smacq_result tabularinput_produce(struct state* state, const dts_object ** datump, int * outchan) {
  int i;
  char * startp, * stopp, * endp, * badp, line[MAX_STR];
  const dts_object * msgdata;
  char * result;
  const dts_object * datum = flow_alloc(state->env, 0, state->empty_type);
  dts_incref(datum, 1);

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
   
    if (i >= state->fields || !state->field_type[i] ||
	!(msgdata = flow_construct_fromstring(state->env, state->field_type[i], strdup(startp))))  {
      double d = strtod(startp, &badp);
      if (badp && badp != endp) {
	//fprintf(stderr, "Double test failed, '%s' remains\n", badp);
	msgdata = flow_construct_fromstring(state->env, state->string_type, strdup(startp));
      } else {
	msgdata = flow_dts_construct(state->env, state->double_type, &d);
      }
    }

    if (!msgdata) 
    assert(msgdata);

    if (i >= state->fields) {
      char buf[1024];
      sprintf(buf, "%d", i+1);
      dts_attach_field(datum, flow_requirefield(state->env, buf), msgdata); 
    } else {
      dts_attach_field(datum, state->field_name[i], msgdata); 
    }

  }

  *datump = datum;
  return(SMACQ_PASS|SMACQ_PRODUCE);
}

static smacq_result tabularinput_consume(struct state * state, const dts_object * datum, int * outchan) {
  assert(0);

  return SMACQ_FREE;
}


static int tabularinput_shutdown(struct state * state) {
  return 0;
}

static int tabularinput_init(struct flow_init * context) {
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
    flow_getoptsbyname(context->argc-1, context->argv+1,
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
  state->field_name = calloc(argc, sizeof(int));
  state->field_type = calloc(argc, sizeof(int));
  
  for (i = 0; i < argc; i++) {
    char * name = strdup(argv[i]);
    char * type;
    type = index(name, ':');
    if (!type) {
      state->field_type[i] = 0;
      state->field_name[i] = flow_requirefield(state->env, name);
    } else {
      type[0] = '\0';
      //fprintf(stderr, "Added field %s type %s\n", name, type+1);
      state->field_name[i] = flow_requirefield(state->env, name);
      state->field_type[i] = flow_requiretype(state->env, type+1);
    }

    free(name);
  }
  
  if (!context->isfirst) {
    fprintf(stderr, "tabularinput is a data source only.\n");
    assert(0);
  }

  state->double_type = flow_requiretype(state->env, "double");
  state->string_type = flow_requiretype(state->env, "string");
  state->empty_type = flow_requiretype(state->env, "empty");

  return 0;
}


/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_tabularinput_table = {
  &tabularinput_produce, 
  &tabularinput_consume,
  &tabularinput_init,
  &tabularinput_shutdown
};

