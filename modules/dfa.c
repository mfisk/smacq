#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <smacq.h>
#include <errno.h>

/* Programming constants */

#define LINESIZE 4096

static struct smacq_options options[] = {
  {NULL, {string_t:NULL}, NULL, 0}
};

struct state {
  smacq_environment * env;
  struct darray machines;
  struct darray states;
  idset ids;
  int start_state;
  int stop_state;
  dts_field previous_field;
}; 

struct dfa {
  int state;
  const dts_object * stack;
};

struct dfa_state {
  struct darray transitions;
};

struct transition {
  smacq_graph * graph;
  int next_state;
  struct runq * runq;
};

static int dfa_try_transition(struct state * state, const dts_object * datum, struct transition * t) {
  const dts_object * output;
  int more;

  smacq_sched_iterative_input(t->graph, datum, t->runq);
  more = smacq_sched_iterative_busy(t->graph, &output, t->runq, 0);

  if (SMACQ_END & more) {
    assert(0);
  }

  if (datum == output) {
    return t->next_state;
  } else if (output) {
    smacq_log("dfa", ERROR, "Transition test was non-boolean");
    return -1;
  } else {
    return -1;
  }
}

static int dfa_try(struct state * state, const dts_object * datum, int current_state) {
  int t;
  struct dfa_state * dstate = darray_get(&state->states, current_state);
  struct darray * transitions = &dstate->transitions;
  
  if (!transitions) {
    smacq_log("dfa", ERROR, "state has no way out");
    fprintf(stderr, "state %d has no way out\n", current_state);
  } else {
    for (t =0; t < darray_size(transitions); t++) {
      int next_state = dfa_try_transition(state, datum, darray_get(transitions, t));
      if (next_state >= 0) {
	return next_state;
      }
    }      
  }
    
  return -1;
}

static smacq_result dfa_consume(struct state * state, const dts_object * datum, int * outchan) {
  int i, t;
  int used = 0;
  int next_state;

  for (i=0; i < darray_size(&state->machines); i++) {
    struct dfa * dfa = darray_get(&state->machines, i);
    if (!dfa) continue;

    dts_incref(dfa->stack, 1);
    dts_attach_field(datum, state->previous_field, dfa->stack);

    next_state = dfa_try(state, datum, dfa->state);

    if (next_state >= 0) { 
      /* This datum caused a transition */

      // fprintf(stderr, "transition from state %d to %d\n", next_state, dfa->state);
      dfa->state = next_state;
      dts_decref(dfa->stack);

      /* Handle STOP states */
      if (state->stop_state == next_state) {
	/* Terminate the DFA */
	darray_set(&state->machines, i, NULL);
	free(dfa);

	return SMACQ_PASS;
      } else {
	dfa->stack = datum;
	dts_incref(datum, 1);
	return SMACQ_FREE;
      }
    } else {
      dts_fieldcache_flush(datum, state->previous_field[0]);
      //assert(state->previous_field[1] == 0);
    }
  }

  /* See if this is the start for a new DFA */
  next_state = dfa_try(state, datum, state->start_state);
  if (next_state >= 0) {
    /* Instantiate a new machine */
    struct dfa * dfa = g_new(struct dfa, 1);
    dfa->state = next_state;
    dfa->stack = datum;
    dts_incref(datum, 1);
    darray_append(&state->machines, dfa);
    //fprintf(stderr, "Cranking up new DFA in state %d\n", next_state);

    return SMACQ_FREE;
  }

  /* This datum wasn't used by any DFA */
  return SMACQ_FREE;
}

static int parse_dfa(struct state * state, char * filename) {
  FILE * fh = fopen(filename, "r");

  if (!fh) {
    smacq_log("dfa", ERROR, strerror(errno));
    return 0;
  }

  idset_init(&state->ids);
  darray_init(&state->states, 2);
  darray_init(&state->machines, 0);
  state->previous_field = smacq_requirefield(state->env, "previous");

  while (!feof(fh)) {
    char line[LINESIZE];
    char * next_state_name, * this_state_name, * test;
    int this_state_num;
    struct transition * transition = NULL;
    struct dfa_state * this_state;

    if (!fgets(line, LINESIZE, fh))
      break;

    this_state_name = line;
    next_state_name = index(line, ' ');
    if (!next_state_name) {
      fprintf(stderr, "error: dfa: cannot parse %s\n", line);
      return -1;
    }
    next_state_name[0] = '\0';
    next_state_name++;

    if (!strcmp(next_state_name, "END")) {
    } else {
      test = index(next_state_name, ' ');
      if (!test) {
	fprintf(stderr, "error: dfa: cannot parse %s\n", line);
	break;
      }
      test[0] = '\0';
      test++;
      
      transition = g_new0(struct transition, 1);
      transition->next_state = str2id(&state->ids, next_state_name);
      this_state_num = str2id(&state->ids, this_state_name);

      {
	char * args[2];
	args[0] = "where";
	args[1] = test;

	transition->graph = smacq_build_query(state->env->types, 2, args);
	
	if (!transition->graph) {
	  fprintf(stderr, "error: dfa: cannot parse test %s\n", test);
	  free(transition);
	  break;
	}

	/*
	  fprintf(stderr, "transition from %s(%d) to %s(%d)\n", 
		this_state_name, this_state_num,
		next_state_name, transition->next_state);
	*/
      }

      if (0 != smacq_start(transition->graph, ITERATIVE, state->env->types)) {
        return SMACQ_ERROR;
      }
      smacq_sched_iterative_init(transition->graph, &transition->runq, 0);
    }

    this_state = darray_get(&state->states, this_state_num);
    if (!this_state) {
      this_state = g_new0(struct dfa_state, 1);
      darray_init(&this_state->transitions, 0);
      darray_set(&state->states, this_state_num, this_state);
    }

    if (transition) darray_append(&this_state->transitions, transition);
  } 

  state->start_state = str2id_try(&state->ids, "START");
  state->stop_state = str2id_try(&state->ids, "STOP");

  if (state->start_state < 0) {
    smacq_log("dfa", ERROR, "No state named START");
    return -1;
  }

  if (state->stop_state < 0) {
    smacq_log("dfa", ERROR, "No state named STOP");
    return -1;
  }

  return 0;
}


static smacq_result dfa_init(struct smacq_init * context) {
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

  assert(argc == 1);

  if (0 != parse_dfa(state, argv[0])) {
    return SMACQ_ERROR;
  }

  return 0;
}

static smacq_result dfa_shutdown(struct state * state) {
  return SMACQ_END;
}


/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_dfa_table = {
  consume: &dfa_consume,
  init: &dfa_init,
  shutdown: &dfa_shutdown
};
