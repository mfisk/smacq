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
  void shutdown(SmacqGraph * f);

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
  bool graphs_alive (SmacqGraph *f);

  void do_shutdown(SmacqGraph *f);
  smacq_result run_produce(SmacqGraph * f);
  bool run_consume(SmacqGraph * f, DtsObject d);

  /// Process a single action or object
  smacq_result element(DtsObject &dout);

  runq q;
};

#include <SmacqGraph.h>

inline void IterativeScheduler::seed_produce(SmacqGraph * startf) {
  while(startf) {
    //fprintf(stderr, "produce_first for %p\n", startf);
    /* Force first guy to produce */
    q.runable(startf, NULL, PRODUCE);
    startf = startf->nextGraph();
  }
}

inline void IterativeScheduler::input(SmacqGraph * g, DtsObject din) {
  assert ((unsigned int)g->instance > 1000);

  for(; g; g=g->nextGraph()) {
    q.runable(g, din, CONSUME);
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
      q.runable(f->children[outchan][i], d, CONSUME);
    }
  } else {
    //fprintf(stderr, "queueing %p falling off leaf %s (%p)\n", d.get(), f->name, f);
    q.runable(NULL, d, CONSUME);
  }
}

inline void IterativeScheduler::do_shutdown(SmacqGraph *f) {
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
	shutdown(child);

      } else if (!child->numparents && child->shutdown) {
	// If we're the last parent to shutdown, and child is already
	// shutdown, then free the child.
	//fprintf(stderr, "delete child %p\n", child);
	//delete child;
      } 
  });
    
  // Propagate to parents
  for (int i=0; i < f->numparents; i++) {
    if (!f->parent[i]->shutdown && !f->parent[i]->live_children()) {
      // No reason to live! 
      shutdown(f->parent[i]);
    }
  }

  if (!f->numparents) {
    // Nobody should reference us anymore
    FOREACH_CHILD(f, assert(!child));
    //fprintf(stderr, "delete self %p\n", f);
    //delete f;
  }
}

inline bool IterativeScheduler::graphs_alive (SmacqGraph *f) {
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

inline void IterativeScheduler::shutdown(SmacqGraph * f) {
  if (!f->shutdown_pending) {
    //fprintf(stderr, "shutdown started for %s(%p)\n", f->argv[0], f);
    f->shutdown_pending = true;
    q.runable(f, NULL, SHUTDOWN);
  }
}

inline smacq_result IterativeScheduler::run_produce(SmacqGraph * f) {
  DtsObject d = NULL;
  int outchan = 0;

  smacq_result pretval = f->instance->produce(d, outchan);

  if (pretval & SMACQ_PASS) {
    queue_children(f, d, outchan);		
  }

  if (pretval & (SMACQ_PRODUCE|SMACQ_CANPRODUCE)) {
    /* Come back for more */
    q.runable(f, NULL, PRODUCE);
  }

  if (pretval & SMACQ_END) {
    assert(!(pretval & (SMACQ_PRODUCE|SMACQ_CANPRODUCE)));

    shutdown(f);
  }

  return pretval;
}
    
/* Return 1 iff the datum was passed */
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
	
  if (retval & (SMACQ_PRODUCE|SMACQ_CANPRODUCE)) {
    run_produce(f);
  }

  if (retval & SMACQ_END) {
    shutdown(f);
  }

  return status;
}


/// Handle one thing on the run queue.
/// Return SMACQ_PASS iff an object falls off the end of the graph.
/// Return SMACQ_NONE iff there is no work we can do.
/// Otherwise, return SMACQ_FREE.
inline smacq_result IterativeScheduler::element(DtsObject &dout) {
  SmacqGraph * f;
  DtsObject d;
  enum action action;
  
  if (!q.pop_runable(f, d, action)) return SMACQ_NONE;

  //fprintf(stderr, "popped %p for %p action %d\n", d.get(), f, action);
  
  if (!f) {
    // Datum fell off end of data-flow graph 
    //fprintf(stderr, "IterativeScheduler::element returning %p\n", d.get());
    
    dout = d;
    return SMACQ_PASS;
  } else if (! f->shutdown) {
    switch (action) {

    case CONSUME:
      //fprintf(stderr, "sched_iterative_element calling consume on %p\n", &d);
      run_consume(f, d);
      break;
      
    case SHUTDOWN:
      if (q.pending(f)) {
	// Defer last call until everything produced and consumed
	q.runable(f, NULL, SHUTDOWN);
      } else {
	// There is nothing pending at all, so it's safe to shutdown
	do_shutdown(f);
      }
      break;
      
    case PRODUCE:
      run_produce(f);
      break;
    }
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
      if (decide(g->children[outchan][i], din)) {
	return SMACQ_PASS;
      }
    }
  }
  return SMACQ_FREE;
}
#endif
