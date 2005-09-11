#ifndef ITERATIVE_SCHEDULER_H
#define ITERATIVE_SCHEDULER_H
#include <stdlib.h>
#include <smacq.h>
#include <RunQ.h>
#include <SmacqGraph.h>
#include <set>

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

  /// Handle an object produced by a currently running node
  void enqueue(SmacqGraph * f, DtsObject d, int outchan);

 private:
  struct ConsumeItem {
    SmacqGraph_ptr g;
    DtsObject d;
   
    // Constructor 
    ConsumeItem() {};
  
    // Copy constructor
    ConsumeItem(ConsumeItem & old) : g(old.g), d(old.d) {};

    // The runq class will set to NULL to remove references
    ConsumeItem(void * v) { assert(!v);};
  };

  /// Place something on the consume queue
  void runable(SmacqGraph *f, DtsObject d);

  bool graphs_alive (SmacqGraph * f);
  void do_delete (SmacqGraph * f);

  smacq_result run_produce(SmacqGraph * f);
  void run_consume(ConsumeItem i);

  /// Process a single action or object
  smacq_result element(DtsObject &dout);

  /// Handle an object produced by the specified node
  void queue_children(SmacqGraph * f, DtsObject d, int outchan);

  smacq_result decide_children(SmacqGraph * g, DtsObject din, int outchan);

  runq<struct ConsumeItem> consumeq;
  runq<SmacqGraph_ptr> produceq;
  runq<DtsObject> outputq;

  std::set<SmacqGraph*> enqueue_stack;
};

#include <SmacqGraph.h>

inline void IterativeScheduler::seed_produce(SmacqGraph * startf) {
  while (startf) {
    // Force first guy to produce
    // We should be a stub and should pass this to children
    assert(!startf->argc);
    FOREACH_CHILD(startf, produceq.enqueue(child));
   
    //produceq.enqueue(startf); // iff argc

    startf = startf->nextGraph();
  }
}

inline void IterativeScheduler::input(SmacqGraph * g, DtsObject din) {
  for(; g; g=g->nextGraph()) {
    if (g->argc) {
    	assert (g->instance > (void*)1000);
 	//struct ConsumeItem i = {g, din};
    	runable(g, din);
    } else {
	// stub
    	FOREACH_CHILD(g, runable(child, din));
    }
  }
}


inline void IterativeScheduler::enqueue(SmacqGraph * caller, DtsObject d, int outchan) {
  queue_children(caller, d, outchan);

  // Modules don't have to be re-entrant, so this makes that safe.
  // It also bounds the stack size sanely.
  enqueue_stack.insert(caller);

  // Schedule some consumes until we would recursively call one of the
  // modules already running. 
  IterativeScheduler::ConsumeItem i;
  while (consumeq.peek(i) && !enqueue_stack.count(i.g.get())) {
    consumeq.pop(i);
    run_consume(i);
  }

  // Don't use produce or output queues here.
  ;

  // Unmark caller and return back to them
  enqueue_stack.erase(caller);
}

inline void IterativeScheduler::runable(SmacqGraph *f, DtsObject d) {
  IterativeScheduler::ConsumeItem i;
  i.g = f;
  i.d = d;
  consumeq.enqueue(i);
}

inline void IterativeScheduler::queue_children(SmacqGraph * f, DtsObject d, int outchan) {
  //fprintf(stderr, "Output channel was %d of %u\n", outchan, f->children.size());
  assert((outchan < (int)f->children.size()) || (!f->children.size() && !outchan));

  assert (f->instance > (void*)1000);

  if (f->children[outchan].size()) {
    //fprintf(stderr, "queueing %p for children of %s (%p)\n", d.get(), f->name, f);
    for (unsigned int i=0; i < f->children[outchan].size(); i++) {
      assert(f->children[outchan][i]);
      //fprintf(stderr, "\tchild %d: %s (%p)\n", i, f->children[outchan][i]->name, f->children[outchan][i]);
      runable(f->children[outchan][i].get(), d);
    }
  } else {
    //fprintf(stderr, "queueing %p falling off leaf %s (%p)\n", d.get(), f->name, f);
    outputq.enqueue(d);
  }
}

/// Try to delete f
inline void IterativeScheduler::do_delete(SmacqGraph * f) {
    assert(!f->numparents);
    FOREACH_CHILD(f, assert(!child));
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
	do_delete(child);
      } 
  });
    
  // Propagate to parents
  for (int i=0; i < f->numparents; i++) {
    if (!f->parent[i]->shutdown && !f->parent[i]->live_children()) {
      // No reason to live! 
      // This shutdown should be processed soon even if there are pending objects for the child.
      // Callee will remove parent/child relationship for us.
      do_shutdown(f->parent[i].get());
      i--; 
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
    ConsumeItem i;
    i.g = f;
    i.d = NULL;
    consumeq.enqueue(i);
}

/// This should destroy the argument, so caller must not refer to it after call.
inline smacq_result IterativeScheduler::run_produce(SmacqGraph * f) {
  int outchan = 0;
  DtsObject d = NULL;

  smacq_result pretval = f->instance->produce(d, outchan);

  if (pretval & SMACQ_PASS) {
    queue_children(f, d, outchan);		
  }

  if (pretval & (SMACQ_PRODUCE|SMACQ_CANPRODUCE)) {
    /* Come back for more */
    produceq.enqueue(f);
  }

  if (pretval & SMACQ_END) {
    assert(!(pretval & (SMACQ_PRODUCE|SMACQ_CANPRODUCE)));

    do_shutdown(f);
  }

  return pretval;
}
    
/// Return true iff the datum was passed.
/// This should destroy the argument, so caller must not refer to it after call.
inline void IterativeScheduler::run_consume(ConsumeItem i) {
  assert(i.g && i.g->instance);

  // Handle already-shutdown case
  if (i.g->shutdown) {
	return;
  }
  if (!i.d) {
	do_shutdown(i.g.get());
	return;
  }

  //fprintf(stderr, "consume %p by %s (%p)\n", i.d.get(), i.g->argv[0], i.g.get());

  int outchan = 0;
  smacq_result retval = i.g->instance->consume(i.d, outchan);

  if (retval & SMACQ_PASS) {
    //fprintf(stderr, "Pass on %p to chan %d by %p\n", i.d.get(), outchan, i.g.get());
    queue_children(i.g.get(), i.d, outchan);
  }
	
  if (retval & SMACQ_END) {
    do_shutdown(i.g.get());
  }
}

/// Handle one thing on the run queue.
/// Return SMACQ_PASS iff an object falls off the end of the graph.
/// Return SMACQ_NONE iff there is no work we can do.
/// Otherwise, return SMACQ_FREE.
inline smacq_result IterativeScheduler::element(DtsObject &dout) {
  SmacqGraph_ptr f;
  ConsumeItem i;

  if (consumeq.pop(i)) {
    run_consume(i);
	
  } else if (produceq.pop(f)) {
    // Only produce if we have nothing else
    if (!f->shutdown) run_produce(f.get());

  } else if (outputq.pop(dout)) {
    // Datum fell off end of data-flow graph
    return SMACQ_PASS;

  } else {
    // All done!
    return SMACQ_NONE;
  }

  // If we did a consume, shutdown, or produce, 
  // See if we can return something too.
  if (outputq.pop(dout)) {
    return SMACQ_PASS;
  } else {
    return SMACQ_FREE;
  }
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

inline smacq_result IterativeScheduler::decide_children(SmacqGraph * g, DtsObject din, int outchan) {
  // Simpler case if top node is a stub
  for (unsigned int i = 0; i < g->children[outchan].size(); i++) {
      if (SMACQ_PASS == decide(g->children[outchan][i].get(), din)) {
	return SMACQ_PASS;
      }
  }
  return SMACQ_FREE;
}

/// Take an input and run it through a boolean graph.
/// Return SMACQ_PASS or SMACQ_FREE.
inline smacq_result IterativeScheduler::decide(SmacqGraph * g, DtsObject din) {
  int outchan = 0;
  assert(g);

  if (!g->argc) return decide_children(g, din, outchan);

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

    return decide_children(g, din, outchan);
  }
  return SMACQ_FREE;
}
#endif
