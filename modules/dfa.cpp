#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <smacq.h>
#include <errno.h>

/* Programming constants */

#define LINESIZE 4096

static struct smacq_options options[] = {
  END_SMACQ_OPTIONS
};

SMACQ_MODULE(dfa,
  PROTO_CTOR(dfa);
  PROTO_CONSUME();

  struct darray machines;
  struct darray states;
  idset ids;
  int start_state;
  int stop_state;
  dts_field previous_field;

  int parse_dfa(char * filename);
  int try_transition(DtsObject datum, struct transition * t);
  int dfa_try(DtsObject datum, int current_state); 
); 

struct dfa {
  int state;
  DtsObject stack;
};

struct dfa_state {
  struct darray transitions;
};

struct transition {
  smacq_graph * graph;
  int next_state;
  struct runq * runq;
};

int dfaModule::try_transition(DtsObject datum, struct transition * t) {
  DtsObject output;
  smacq_result more;

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

int dfaModule::dfa_try(DtsObject datum, int current_state) {
  int t;
  struct dfa_state * dstate = (struct dfa_state*)darray_get(&states, current_state);
  struct darray * transitions = &dstate->transitions;
  
  if (!transitions) {
    //smacq_log("dfa", ERROR, "state has no way out");
    fprintf(stderr, "state %d has no way out\n", current_state);
  } else {
    for (t =0; t < darray_size(transitions); t++) {
      int next_state = try_transition(datum, (transition*)darray_get(transitions, t));
      if (next_state >= 0) {
	return next_state;
      }
    }      
  }
    
  return -1;
}

smacq_result dfaModule::consume(DtsObject datum, int * outchan) {
  int i;
  int next_state;

  for (i=0; i < darray_size(&machines); i++) {
    struct dfa * dfa = (struct dfa*)darray_get(&machines, i);
    if (!dfa) continue;

    
    datum->attach_field(previous_field, dfa->stack);

    next_state = dfa_try(datum, dfa->state);

    if (next_state >= 0) { 
      /* This datum caused a transition */

      // fprintf(stderr, "transition from state %d to %d\n", next_state, dfa->state);
      dfa->state = next_state;
      

      /* Handle STOP states */
      if (stop_state == next_state) {
	/* Terminate the DFA */
	darray_set(&machines, i, NULL);
	free(dfa);

	return SMACQ_PASS;
      } else {
	dfa->stack = datum;
	
	return SMACQ_FREE;
      }
    } else {
      //datum->fieldcache_flush(previous_field[0]);
      ////assert(previous_field[1] == 0);
    }
  }

  /* See if this is the start for a new DFA */
  next_state = dfa_try(datum, start_state);
  if (next_state >= 0) {
    /* Instantiate a new machine */
    struct dfa * dfa = g_new(struct dfa, 1);
    dfa->state = next_state;
    dfa->stack = datum;
    
    darray_append(&machines, dfa);
    //fprintf(stderr, "Cranking up new DFA in state %d\n", next_state);

    return SMACQ_FREE;
  }

  /* This datum wasn't used by any DFA */
  return SMACQ_FREE;
}

int dfaModule::parse_dfa(char * filename) {
  FILE * fh = fopen(filename, "r");

  if (!fh) {
    smacq_log("dfa", ERROR, strerror(errno));
    return 0;
  }

  idset_init(&ids);
  darray_init(&states, 2);
  darray_init(&machines, 0);
  previous_field = dts->requirefield("previous");

  while (!feof(fh)) {
    char line[LINESIZE];
    char * next_state_name, * this_state_name, * test;
    struct transition * transition = NULL;
    int this_state_num = -1;
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
      this_state_num = str2id(&ids, this_state_name);
      transition->next_state = str2id(&ids, next_state_name);

      {
	char * args[2];
	args[0] = "where";
	args[1] = test;

	transition->graph = smacq_build_query(dts, 2, args);
	
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

      if (0 != smacq_start(transition->graph, ITERATIVE, dts)) {
        return -1;
      }
      smacq_sched_iterative_init(transition->graph, &transition->runq, 0);
    }

    if (this_state_num > -1) {
    	this_state = (dfa_state*)darray_get(&states, this_state_num);
    	if (!this_state) {
      		this_state = g_new0(struct dfa_state, 1);
      		darray_init(&this_state->transitions, 0);
      		darray_set(&states, this_state_num, this_state);
    	}

    	if (transition) {
      	darray_append(&this_state->transitions, transition);
    	}
    }
  } 

  start_state = str2id_try(&ids, "START");
  stop_state = str2id_try(&ids, "STOP");

  if (start_state < 0) {
    smacq_log("dfa", ERROR, "No state named START");
    return -1;
  }

  if (stop_state < 0) {
    smacq_log("dfa", ERROR, "No state named STOP");
    return -1;
  }

  return 0;
}


dfaModule::dfaModule(struct smacq_init * context) : SmacqModule(context) {
  int argc = context->argc-1;
  char ** argv = context->argv+1;

  assert(argc == 1);

  if (0 != parse_dfa(argv[0])) {
    assert(0);
  }
}
