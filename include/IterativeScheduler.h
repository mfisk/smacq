#ifndef ITERATIVE_SCHEDULER_H
#define ITERATIVE_SCHEDULER_H
#include <IterativeScheduler-interface.h>
#include <SmacqGraph.h>

inline void IterativeScheduler::seed_produce(SmacqGraph * startf) {
   startf->mustProduce = true;
   produceq.enqueue(startf);
}
	
inline void IterativeScheduler::seed_produce(SmacqGraphContainer * startf) {
  for (unsigned int h = 0; h < startf->head.size(); h++) {
    // Force first guy to produce
    // We should be a stub and should pass this to children
    if (!startf->head[h]->argc) {
    	FOREACH_CHILD(startf->head[h], produceq.enqueue(child));
    } else {
	seed_produce(startf->head[h].get());
    }
  }
}

inline void IterativeScheduler::input(SmacqGraphContainer * c, DtsObject din) {
  for (unsigned int h = 0; h < c->head.size(); h++) {
    SmacqGraph * g = c->head[h].get();
    if (g->argc) {
    	assert (g->instance > (void*)1000);
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
  while (run_consume()) ;

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
    //fprintf(stderr, "queueing %p for children of %s (%p)\n", d.get(), f->argv[0], f);
    for (unsigned int i=0; i < f->children[outchan].size(); i++) {
      assert(f->children[outchan][i]);
      //fprintf(stderr, "\tchild %d: %s (%p)\n", i, f->children[outchan][i]->argv[0], f->children[outchan][i].get());
      runable(f->children[outchan][i].get(), d);
    }
  } else {
    //fprintf(stderr, "queueing %p falling off leaf %s (%p)\n", d.get(), f->name, f);
    outputq.enqueue(d);
  }
}

inline void IterativeScheduler::run_produce(SmacqGraph * f) {
  int outchan = 0;
  DtsObject d = NULL;
  smacq_result pretval;

  // So here's the problem:  If run_produce returns SMACQ_PRODUCE, then we 
  // cannot run consume on this module until we have finished producing.
  // But we don't want to produce too eagerly before letting other modules
  // consume (or else we may buffer too much stuff in memory).

  f->mustProduce = false; 

  if (f->shutdown) return;

  assert(f->instance);
  pretval = f->instance->produce(d, outchan);

  if (pretval & SMACQ_PASS) {
      enqueue(f, d, outchan);		
  }

  if (pretval & SMACQ_END) {
      //assert(!(pretval & (SMACQ_PRODUCE|SMACQ_CANPRODUCE)));
 
      //fprintf(stderr, "module %p asked for shutdown on produce\n", f); 
      SmacqGraph::do_shutdown(f);
  } else if (pretval & (SMACQ_PRODUCE|SMACQ_CANPRODUCE)) {
      seed_produce(f); // Keep producing
  }
}
    
/// Return true iff the datum was passed.
/// This should destroy the argument, so caller must not refer to it after call.
inline bool IterativeScheduler::run_consume() {
  ConsumeItem i;
  if (!consumeq.peek(i)) return false;
  if (enqueue_stack.count(i.g.get())) return false;

  assert(i.g);

  if (i.g->mustProduce) {
      run_produce(i.g.get());
      return true;
  } 

  consumeq.pop(i);

  // Handle already-shutdown case
  if (i.g->shutdown) {
	return true;
  }

  assert(i.g->instance);
  assert(i.d); // No more scheduled shutdowns

  //fprintf(stderr, "consume %p by %s (%p)\n", i.d.get(), i.g->argv[0], i.g.get());

  int outchan = 0;
  smacq_result retval = i.g->instance->consume(i.d, outchan);

  if (retval & SMACQ_PASS) {
    //fprintf(stderr, "Pass on %p to chan %d by %p\n", i.d.get(), outchan, i.g.get());
    queue_children(i.g.get(), i.d, outchan);
  }

  if (retval & (SMACQ_PRODUCE|SMACQ_CANPRODUCE)) {
    run_produce(i.g.get());
  }
	
  if (retval & SMACQ_END) {
    //fprintf(stderr, "module %p asked for shutdown on consume\n", i.g.get()); 
    SmacqGraph::do_shutdown(i.g.get());
  }

  return true;
}

/// Handle one thing on the run queue.
/// Return SMACQ_PASS iff an object falls off the end of the graph.
/// Return SMACQ_NONE iff there is no work we can do.
/// Otherwise, return SMACQ_FREE.
inline smacq_result IterativeScheduler::element(DtsObject &dout) {
  SmacqGraph_ptr f;

  if (run_consume()) {
    ;
  } else if (produceq.pop(f)) {
    // Only produce if we have nothing else
    run_produce(f.get());

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
inline smacq_result IterativeScheduler::decide(SmacqGraphContainer * g, DtsObject din) {
  for (unsigned int i = 0; i < g->head.size(); i++) {
	if (SMACQ_PASS == decide(g->head[i].get(), din)) {
		return SMACQ_PASS;
	};
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
