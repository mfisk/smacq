#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
#include <smacq-internal.h>
#include <fields.h>
#include <bytehash.h>

/* Programming constants */

#define KEYBYTES 128

static struct smacq_options options[] = {
  {NULL, {string_t:NULL}, NULL, 0}
};

struct join {
  struct filter * graph;
  int field;
  void * runq;
};

struct state {
  smacq_environment * env;
  int numjoins;
  int whereargc;
  char ** whereargv;
  const dts_object * product;

  struct join * joins;
}; 

static smacq_result join_consume(struct state * state, const dts_object * datum, int * outchan) {
  int more;
  int i;

  for (i=0; i<state->numjoins; i++) {
	struct join * j = &state->joins[i];
  	const dts_object * product;

  	more = smacq_sched_iterative(j->graph, NULL, &product, &j->runq, 1);
	dts_attach_field(datum, j->field, product);
	//fprintf(stderr, "attaching %p to %p field %d\n", product, datum, j->field);
  }
  
  return SMACQ_PASS;
}

static int join_init(struct smacq_init * context) {
  int argc = 0;
  char ** argv;
  int i;
  struct state * state = context->state = g_new0(struct state, 1);
  state->env = context->env;
  argc=context->argc-1;
  argv=context->argv+1;

  state->whereargc = 0;
  for (i=0; i<argc; i++) {
    if (!strcmp(argv[i], "--")) {
      state->whereargc = argc - i - 1;
      state->whereargv = argv+(i+1);
      break;
    }
  }

  argc -= (state->whereargc);

  // Consume rest of arguments as joins
  assert(argc > 0);

  state->joins = calloc(argc, sizeof(struct join));
  state->numjoins = argc; 

  for (i=0; i<argc; i++) {
	  char fstr[256];
	  snprintf(fstr, 256, "j%d", i+1);
	  state->joins[i].field = smacq_requirefield(state->env, fstr);
	  state->joins[i].graph = smacq_build_query(1, &argv[i]);
	  smacq_start(state->joins[i].graph, ITERATIVE, state->env->types);
  }
	

  {
  	struct smacq_optval optvals[] = {
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(argc, argv,
			       &argc, &argv,
			       options, optvals);

  }

  return 0;
}

static int join_shutdown(struct state * state) {
  return SMACQ_END;
}


static smacq_result join_produce(struct state * state, const dts_object ** datump, int * outchan) {
  int status;

  if (state->product) {
    *datump = state->product;
    status = SMACQ_PASS;
  } else {
    status = SMACQ_FREE;
  }

  return status | (state->product ? SMACQ_PRODUCE : 0);
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_join_table = {
  &join_produce, 
  &join_consume,
  &join_init,
  &join_shutdown
};
