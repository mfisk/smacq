#ifndef ITERATIVE_SCHEDULER_H
#define ITERATIVE_SCHEDULER_H
#include <stdlib.h>
#include <smacq.h>
#include <RunQ.h>

/// This is currently the only scheduler implementation.
class IterativeScheduler {
public:
  
  /// A default graph must be specified.  Graph graph's init() method
  /// is called before anything else is done.  Iff produce_first is
  IterativeScheduler() {};

  /// Cue the head(s) of the given graph to start producing data.
  /// Otherwise data must be provided using the input() method.
  void seed_produce(SmacqGraph*);

  /// Queue an object for input to the specified graph.
  void input(SmacqGraph * g, DtsObject din);

  /// Queue a shutdown for the specified graph.
  void sched_shutdown(SmacqGraph * f);

  /// Perform an immediate shutdown for the specified graph.  The argument is invalidated.
  void do_shutdown(SmacqGraph * f);

  /// Run until an output object is ready.
  smacq_result get(DtsObject &dout);

  /// Process a single action or object
  smacq_result IterativeScheduler::decide(SmacqGraph *, DtsObject din);

  /// Run to completion.  
  /// Return false iff error.
  bool busy_loop();

  /// Handle an object produced by the specified node
  void queue_children(SmacqGraph * f, DtsObject d, int outchan);

 private:
  bool graphs_alive (SmacqGraph * f);
  void do_delete (SmacqGraph * f);

  smacq_result run_produce(SmacqGraph * f);
  bool run_consume(SmacqGraph * f, DtsObject d);

  /// Process a single action or object
  smacq_result element(DtsObject &dout);

  runq consumeq, produceq;
};

#include <SmacqGraph.h>

inline void IterativeScheduler::seed_produce(SmacqGraph * startf) {
  while(startf) {
    //fprintf(stderr, "produce_first for %p\n", startf);
    /* Force first guy to produce */
    produceq.runable(startf, NULL);
    startf = startf->nextGraph();
  }
}

inline void IterativeScheduler::input(SmacqGraph * g, DtsObject din) {
  assert ((unsigned int)g->instance > 1000);

  for(; g; g=g->nextGraph()) {
    consumeq.runable(g, din);
  }
}


inline void IterativeScheduler::queue_children(SmacqGraph * f, DtsObject d, int outchan) {
  assert(outchan < (int)f->children.size());

  assert ((unsigned int)f->instance > 1000);

  if (f->children[outchan].size()) {
    //fprintf(stderr, "queueing %p for children of %s (%p)\n", d.get(), f->name, f);
    for (unsigned int i=0; i < f->children[outchan].size(); i++) {
      assert(f->children[outchan][i]);
      //fprintf(stderr, "\tchild %d: %s (%p)\n", i, f->children[outchan][i]->name, f->children[outchan][i]);
      consumeq.runable(f->children[outchan][i].get(), d);
    }
  } else {
    //fprintf(stderr, "queueing %p falling off leaf %s (%p)\n", d.get(), f->name, f);
    consumeq.runable(NULL, d);
  }
}

/// Try to delete f
inline void IterativeScheduler::do_delete(SmacqGraph * f) {
    assert(!f->numparents);
    FOREACH_CHILD(f, assert(!child));

    // XXX: These are expensive!
    produceq.remove(f);
    consumeq.remove(f);

    /*
    if (consumeq.find(f)) {
	fprintf(stderr, "Warning: cannot delete %p because still on consume queue\n", f);
    } else if (produceq.find(f)) {
	fprintf(stderr, "Warning: cannot delete %p because still on produce queue\n", f);
    } else {
    	//fprintf(stderr, "delete self %p\n", f);
    	//delete f;
    }
    */
}


/// This should destroy the argument, so caller must not refer to it after call.
inline void IterativeScheduler::do_shutdown(SmacqGraph * f) {
  if (f->shutdown) {
    // Already shutdown, so do nothing 
    return;
  }

  f->shutdown = true;
  delete f->instance; // Call the destructor, which may callback to enqueue()
  f->instance = NULL;

  FOREACH_CHILD(f, {
      // Couldn't remove until after instance destroyed.
      // Now it won't matter if we start to look like a leaf.
      f->remove_child(i,j);
      j--; //fixup iterator
      
      if (!child->shutdown && !child->live_parents()) {
	// XXX. Child could also be a head that shouldn't be shutdown!
	// This shutdown should occur AFTER any pending objects are processed by the child.
	sched_shutdown(child);

      } else if (!child->numparents && child->shutdown) {
	// If we're the last parent to shutdown, and child is already
	// shutdown, then free the child.
	//fprintf(stderr, "delete child %p\n", child);
	do_delete(f);
      } 
  });
    
  // Propagate to parents
  for (int i=0; i < f->numparents; i++) {
    if (!f->parent[i]->shutdown && !f->parent[i]->live_children()) {
      // No reason to live! 
      // This shutdown should be processed soon even if there are pending objects for the child.
      // Callee will remove parent/child relationship for us.
      do_shutdown(f->parent[i].get());
    }
  }

  if (!f->numparents) {
    // Nobody should reference us anymore
    do_delete(f);
  }
}

inline bool IterativeScheduler::graphs_alive (SmacqGraph * f) {
  if (! f->shutdown) {
    return true;
  }
  
  FOREACH_CHILD(f, {
      if (child) {
	if (graphs_alive(child)) {
	  return true;
	}
      }
    })

    return false;
}

inline void IterativeScheduler::sched_shutdown(SmacqGraph * f) {
    //fprintf(stderr, "shutdown started for %s(%p)\n", f->argv[0], f);
    consumeq.runable(f, NULL);
}

/// This should destroy the argument, so caller must not refer to it after call.
inline smacq_result IterativeScheduler::run_produce(SmacqGraph * f) {
  DtsObject d = NULL;
  int outchan = 0;

  smacq_result pretval = f->instance->produce(d, outchan);

  if (pretval & SMACQ_PASS) {
    queue_children(f, d, outchan);		
  }

  if (pretval & (SMACQ_PRODUCE|SMACQ_CANPRODUCE)) {
    /* Come back for more */
    produceq.runable(f, NULL);
  }

  if (pretval & SMACQ_END) {
    assert(!(pretval & (SMACQ_PRODUCE|SMACQ_CANPRODUCE)));

    do_shutdown(f);
  }

  return pretval;
}
    
/// Return 1 iff the datum was passed.
/// This should destroy the argument, so caller must not refer to it after call.
inline bool IterativeScheduler::run_consume(SmacqGraph * f, DtsObject d) {
  int outchan = 0;
  smacq_result retval;
  int status = false;

  //fprintf(stderr, "consume %p by %s (%p)\n", d.get(), f->name, f);

  //assert(f && f->instance);
  retval = f->instance->consume(d, outchan);

  if (retval & SMACQ_PASS) {
    //fprintf(stderr, "Pass on %p by %p\n", d.get(), f);
    queue_children(f, d, outchan);
    status = true;
  }
	
  if (retval & SMACQ_END) {
    do_shutdown(f);
  }

  return status;
}


/// Handle one thing on the run queue.
/// Return SMACQ_PASS iff an object falls off the end of the graph.
/// Return SMACQ_NONE iff there is no work we can do.
/// Otherwise, return SMACQ_FREE.
inline smacq_result IterativeScheduler::element(DtsObject &dout) {
  SmacqGraph_ptr f;
  DtsObject d;
 
  if (consumeq.pop_runable(f,d)) {
      if (!f) {
    	// Datum fell off end of data-flow graph 
    	dout = d;
    	return SMACQ_PASS;
      } else if (!f->shutdown) {
	if (!d) {
		do_shutdown(f.get());
	} else {
		run_consume(f.get(), d);
	}
      }
  } else if (produceq.pop_runable(f,d)) {
      if (!f->shutdown) {
        run_produce(f.get());
      }
  } else {
      return SMACQ_NONE;
  }

  return SMACQ_FREE;
}

/// Process until completion.  Returns unless there is an error.
inline bool IterativeScheduler::busy_loop() {
  DtsObject dout; // Ignored.
  smacq_result r;
  smacq_result prev = SMACQ_NONE; 
  for (;;) {
    r = element(dout);

    if (r == SMACQ_NONE) {
      break;
    } else {
      prev = r;
    }
  } 

  if (prev & SMACQ_ERROR) {
    return false;
  } else {
    return true;
  }
}

inline smacq_result IterativeScheduler::get(DtsObject &dout) {
  dout = NULL;
  smacq_result r;
  smacq_result done = SMACQ_ERROR|SMACQ_END|SMACQ_PASS;
  do {
    r = element(dout);
  } while (r != SMACQ_NONE && ! (r & done));

  return r;
}

/// Take an input and run it through a boolean graph.
/// Return SMACQ_PASS or SMACQ_FREE.
inline smacq_result IterativeScheduler::decide(SmacqGraph * g, DtsObject din) {
  int outchan = 0;
  assert(g);
  assert(g->instance);
  smacq_result r = g->instance->consume(din, outchan);

  assert(! (r & (SMACQ_PRODUCE|SMACQ_CANPRODUCE|SMACQ_END)));

  if (r & SMACQ_PASS) {
    //fprintf(stderr, "Pass on %p by %p\n", d.get(), f);
    assert(outchan < (int)g->children.size());

    // Base case:  got to an end of graph!
    if (!g->children[outchan].size()) {
      return SMACQ_PASS;
    }

    // Try each child
    for (unsigned int i = 0; i < g->children[outchan].size(); i++) {
      if (decide(g->children[outchan][i].get(), din)) {
	return SMACQ_PASS;
      }
    }
  }
  return SMACQ_FREE;
}
#endif
