#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "smacq.h"

struct mask {
	in_addr_t mask;
	in_addr_t addr;
	int not;
	/* struct mask * next; */
};

struct state {
  smacq_environment * env;
  dts_field field;
  struct mask mask;
};

static struct smacq_options options[] = {
  //{"f", {string_t:NULL}, "Field to inspect (full data is default)", SMACQ_OPT_TYPE_STRING},
  //{"m", {boolean_t:0}, "OR multiple fields and demux to individual outputs", SMACQ_OPT_TYPE_BOOLEAN},
  {NULL, {NULL}, NULL, 0}
};

static smacq_result mask_produce(struct state* state, const dts_object ** datum, int * outchan) {
  return SMACQ_ERROR;
}

static smacq_result mask_consume(struct state * state, const dts_object * datum, int * outchan) {
  const dts_object * field;
  in_addr_t f;
  int found = 0;

  assert(datum);

  if (state->field) {
  	field = smacq_getfield(state->env, datum, state->field, NULL);
  	if (!field) return SMACQ_FREE;

        f = dts_data_as(field, in_addr_t);
  	dts_decref(field);
  } else {
        f = dts_data_as(datum, in_addr_t);
  }
 
  if ((f & state->mask.mask) == state->mask.addr) {
	  found = 1;
  }

  if (state->mask.not) {
	  return (found ? SMACQ_FREE : SMACQ_PASS);
  } else {
	  return (found ? SMACQ_PASS : SMACQ_FREE);
  }
}

static int mask_init(struct smacq_init * context) {
  struct state * state;
  int i, argc;
  char ** argv;

  context->state = state = (struct state*) calloc(sizeof(struct state),1);
  assert(state);

  state->env = context->env;
  {
    struct smacq_optval optvals[] = {
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &argc, &argv,
				 options, optvals);
  }

  state->field = smacq_requirefield(state->env, argv[0]);

  assert(argc==2);
  for (i = 1; i < argc; i++) {
	  char * slash = index(argv[i], '/');
	  char * not = index(argv[i], '!');
	  struct in_addr addr;

	  int cidr;

	  if (not && (not == argv[i])) {
		  state->mask.not = 1;
		  argv[i]++;
   	  }

	  if (slash) {
		  slash[0] = '\0';
		  cidr = atof(slash+1);
	  } else {
		  cidr = 32;
	  }

	  state->mask.mask = (~0) << (32 - cidr);
	  if (inet_aton(argv[i], &addr)) {
	  	state->mask.addr = addr.s_addr;
	  } else {
		fprintf(stderr, "mask: Unable to parse address %s.\n", argv[i]);
		return SMACQ_ERROR|SMACQ_END;
	  }
  }

  return SMACQ_PASS;
}

static int mask_shutdown(struct state * state) {
  free(state);
  return SMACQ_END;
}

struct smacq_functions smacq_mask_table = {
	produce: &mask_produce, 
	consume: &mask_consume,
	init: &mask_init,
	shutdown: &mask_shutdown,
};

