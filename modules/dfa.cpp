#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <smacq.h>
#include <errno.h>
#include <SmacqScheduler.h>
#include <DynamicArray.h>

#define LINESIZE 4096

static struct smacq_options options[] = {
  END_SMACQ_OPTIONS
};

#include <ext/hash_map>
namespace stdext = ::__gnu_cxx;

namespace std {
  template<>
  struct equal_to<char*> {
	bool operator()(const char * x, const char * y) {
		return (!strcmp(x,y));
	}
  };
}

template <class T>
class IdMap : public stdext::hash_map<T, int> {
   public:
  	IdMap() : ids(0) {}

        int operator[](const T & x) {
		if (find(x) == end()) {
			// New
                	int & i = stdext::hash_map<T,int>::operator[](strdup(x));
			i = ids;
			return ids++;
		} else {
			return (find(x)->second);
		}
        }

   private:
	int ids;
};

SMACQ_MODULE(dfa,
  PROTO_CTOR(dfa);
  PROTO_CONSUME();

  DynamicArray<struct dfa> machines;
  DynamicArray<struct dfa_state> states;
  int start_state;
  int stop_state;
  dts_field previous_field;

  IdMap<char *> ids;

  int parse_dfa(char * filename);
  int try_transition(DtsObject datum, struct transition & t);
  int dfa_try(DtsObject datum, int current_state); 
); 

struct dfa {
  int state;
  DtsObject stack;
};

struct dfa_state {
  DynamicArray<struct transition> transitions;
};

struct transition {
  SmacqGraph * graph;
  int next_state;
  SmacqScheduler * sched;
};

int dfaModule::try_transition(DtsObject datum, struct transition & t) {
  DtsObject output;
  smacq_result more;

  t.graph->print(stderr, 2);
  more = t.sched->decide(datum);

  if ((SMACQ_END|SMACQ_ERROR) & more) {
    assert(0);
  }

  if (more & SMACQ_PASS) {
    	return t.next_state;
  }

  return -1;
}

int dfaModule::dfa_try(DtsObject datum, int current_state) {
  struct dfa_state & dstate = states[current_state];
  
  if (dstate.transitions.empty()) {
    //smacq_log("dfa", ERROR, "state has no way out");
    fprintf(stderr, "state %d has no way out\n", current_state);
  } else {
    for (unsigned int t = 0; t < dstate.transitions.size(); t++) {
      int next_state = try_transition(datum, dstate.transitions[t]);
      if (next_state >= 0) {
	return next_state;
      }
    }      
  }
    
  return -1;
}

smacq_result dfaModule::consume(DtsObject datum, int & outchan) {
  int next_state;
  DynamicArray<struct dfa>::iterator i;

  for (i = machines.begin(); i != machines.end(); ++i) {
    struct dfa & dfa = *i;
    
    datum->attach_field(previous_field, dfa.stack);

    next_state = dfa_try(datum, dfa.state);

    if (next_state >= 0) { 
      /* This datum caused a transition */

      // fprintf(stderr, "transition from state %d to %d\n", next_state, dfa.state);
      dfa.state = next_state;
      

      /* Handle STOP states */
      if (stop_state == next_state) {
	/* Terminate the DFA */
	machines.erase(i);

	return SMACQ_PASS;
      } else {
	dfa.stack = datum;
	
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
    struct dfa & ndfa = machines[machines.size()];
    ndfa.state = next_state;
    ndfa.stack = datum;
    
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

  previous_field = dts->requirefield("previous");

  while (!feof(fh)) {
    char line[LINESIZE];
    char * next_state_name, * this_state_name, * test;
    int this_state_num = -1;

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
      
      this_state_num = ids[this_state_name];
      DynamicArray<struct transition> & transitions = states[this_state_num].transitions;
      struct transition & transition = transitions[transitions.size()];

      transition.next_state = ids[next_state_name];

      {
	char * args[2];
	args[0] = "where";
	args[1] = test;

	transition.graph = SmacqGraph::newQuery(dts, 2, args);

	//transition.graph->print(stderr, 2);
	
	if (!transition.graph) {
	  fprintf(stderr, "error: dfa: cannot parse test %s\n", test);
	  exit(-1);
	}

	/*
	  fprintf(stderr, "transition from %s(%d) to %s(%d)\n", 
		this_state_name, this_state_num,
		next_state_name, transition.next_state);
	*/
      }

      transition.sched = new SmacqScheduler(dts, transition.graph, false);
    }
    assert(this_state_num > -1);

  } 

  IdMap<char*>::iterator i = ids.find("START");

  if (i == ids.end()) {
    smacq_log("dfa", ERROR, "No state named START");
    return -1;
  } else {
    start_state = i->second;
  }

  i = ids.find("STOP");

  if (i == ids.end()) {
    smacq_log("dfa", ERROR, "No state named STOP");
    return -1;
  } else {
    stop_state = i->second;
  }

  return 0;
}


dfaModule::dfaModule(struct SmacqModule::smacq_init * context) : SmacqModule(context) {
  int argc = context->argc-1;
  char ** argv = context->argv+1;

  assert(argc == 1);

  if (0 != parse_dfa(argv[0])) {
    assert(0);
  }
}
