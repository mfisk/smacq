#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
#include <smacq.h>
#include <fields.h>
#include <bytehash.h>

static struct smacq_options options[] = {
  {NULL, {string_t:NULL}, NULL, 0}
};

struct state {
  smacq_environment * env;
  struct clause * clause;
  int num_clauses;
  int num_active_clauses;
}; 

struct clause {
  struct runq * runq;
  smacq_graph * graph;
};

int build_clause(struct state * state, char ** argv, int num) {
		  /* End of query */
		  smacq_graph * g;
		  if (num < 1) {
			  fprintf(stderr, "OR: empty clause\n");
			  return 0;
		  }

		  state->num_clauses++;
		  state->clause = realloc(state->clause, state->num_clauses * sizeof(struct clause));
		  g = smacq_build_query(state->env->types, num, argv);
		  if (0 != smacq_start(g, ITERATIVE, state->env->types)) {
			  return 0;
		  } 
		  state->clause[state->num_clauses-1].graph = g;
		  state->clause[state->num_clauses-1].runq = NULL;

		  return 1;
}

static smacq_result or_consume(struct state * state, const dts_object * datum, int * outchan) {
  int i;
  int status = SMACQ_FREE;

  for (i=0; i < state->num_clauses; i++) {
  	const dts_object * output = NULL;
	struct clause * j = &state->clause[i];
	int more;

	if (!j) continue;

  	fprintf(stderr, "run graph %d\n", i);
  	{
	  more = smacq_sched_iterative(j->graph, datum, &output, &j->runq, 0);

	  if (SMACQ_END & more) {
		fprintf(stderr, "%d ended\n", i);
		free(j);
		state->clause[i].graph = NULL;
		state->num_active_clauses--;
		break;
	  }
	} //while (j->runq);

  	fprintf(stderr, "ran graph %d, output %p\n", i, output);
	fprintf(stderr, "Passed in %p, got %p out:\n", datum, output);

	if (output == datum) {
		status = SMACQ_PASS;
		break;
	} else if (output) {
		fprintf(stderr, "Error OR called on non-boolean function!\n");
	}
  }

  if (state->num_active_clauses < 1) {
	status |= SMACQ_END;
  }
  return status;
}

static smacq_result or_init(struct smacq_init * context) {
  int argc = 0;
  char ** argv;

  struct state * state = context->state = calloc(1, sizeof(struct state));
  state->env = context->env;

  {
  	struct smacq_optval optvals[] = {
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

  }

  {
  	int start = 0;
	int i;
  	for (i=0; i<argc; i++) {
	  if (!strcmp(argv[i], ";")) {
		  if (!build_clause(state, argv, i - start))
			  return SMACQ_END;
		  start = i+1;
	  }
	}
	if (start < argc)
		build_clause(state, argv, argc - start);
  }

  state->num_active_clauses = state->num_clauses;

  fprintf(stderr, "init done with %d clauses\n", state->num_active_clauses);

  return 0;
}

static smacq_result or_shutdown(struct state * state) {
  int i;
  for (i=0; i<state->num_clauses; i++) {
	  struct clause * j = &state->clause[i];
	  if (j) {
		  smacq_sched_iterative_shutdown(j->graph, j->runq);
	  }
  }
  return SMACQ_END;
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_lor_table = {
	 consume: &or_consume,
	    init: &or_init,
	shutdown: &or_shutdown,
	 algebra: { boolean: 1}
};
