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
  /// true, then when there is nothing else in the runq, the head(s)
  /// of the default graph will be asked to produce new data.
  /// Otherwise data must be provided using the input() method.
  IterativeScheduler(DTS *, SmacqGraph *, bool produce_first);

  /// Queue an object for input to the default graph.
  void input(DtsObject din);

  /// Queue an object for input to the specified graph.
  void input(SmacqGraph * g, DtsObject din);

  /// Queue a shutdown for the specified graph.
  void shutdown(SmacqGraph * f);

  /// Run until an output object is ready.
  smacq_result get(DtsObject &dout);

  /// Process a single action or object
  smacq_result IterativeScheduler::decide(DtsObject din);

  /// Run to completion.  
  /// Return false iff error.
  bool busy_loop();

  /// Handle an object produced by the specified node
  void queue_children(SmacqGraph * f, DtsObject d, int outchan);

 private:
  bool graphs_alive (SmacqGraph *f);

  void do_shutdown(SmacqGraph *f);
  void check_for_shutdown(SmacqGraph *f);
  smacq_result run_produce(SmacqGraph * f);
  bool run_consume(SmacqGraph * f, DtsObject d);

  /// Process a single action or object
  smacq_result element(DtsObject &dout);

  runq q;
  SmacqGraph * graph;
};

#include <SmacqGraph.h>

inline IterativeScheduler::IterativeScheduler(DTS * dts, 
				       SmacqGraph * startf, 
				       bool produce_first) 
  : graph(startf)
{
  startf->init(dts, this);

  if (produce_first) {
    while(startf) {
      //fprintf(stderr, "produce_first for %p\n", startf);
      /* Force first guy to produce */
      q.runable(startf, NULL, PRODUCE);
      startf = startf->nextGraph();
    }
  }
}

inline void IterativeScheduler::input(DtsObject din) {
  input(graph, din);
}

inline void IterativeScheduler::input(SmacqGraph * g, DtsObject din) {
  for(; g; g=g->nextGraph()) {
    q.runable(g, din, CONSUME);
  }
}


inline void IterativeScheduler::queue_children(SmacqGraph * f, DtsObject d, int outchan) {
	assert(outchan < (int)f->children.size());

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

inline void IterativeScheduler::check_for_shutdown(SmacqGraph *f) {
  // Already shutdown
  if (f->status & SMACQ_FREE) return;

  // Give up if any of our children are still around
  FOREACH_CHILD(f, {
      if (child && (! (child->status & SMACQ_FREE))) 
	return;
    });

  // No more active children.  Clean-up self and parents. 
  for (int i=0; i < f->numparents; i++) {
    SmacqGraph * p = f->parent[i];
    assert(p);
    if (! (p->status & SMACQ_FREE)) {
 	p->remove_childgraph(f);
	i--;
	if (!p->numchildren()) {
      		shutdown(p);
	}
    }
  }
  this->shutdown(f);
}

inline void IterativeScheduler::do_shutdown(SmacqGraph *f) {
  if (f->status == SMACQ_FREE) {
	/* Already shutdown, so do nothing */
	return;
  }
  //fprintf(stderr, "shutdown followup for %p\n", f);

  f->status = SMACQ_FREE;
  delete f->instance; // This will call the destructor, which may callback to enqueue()
  f->instance = NULL;

  /* Propagate to children */
  FOREACH_CHILD(f, {
      if (! (child->status & SMACQ_FREE)) {
	//fprintf(stderr, "Child %p down from %d to", child, child->numparents);
	f->remove_child(i,j); 

	// Fixup iterator
	--j;
	
	//fprintf(stderr, " %d parents\n", child->numparents);
	if (!child->numparents) {
  		/* XXX. Child could also be a head that shouldn't be shutdown! */
		shutdown(child);

	}
      }
  })

  /* Propagate to parents */
  for (int i=0; i< f->numparents; i++) {
    check_for_shutdown(f->parent[i]);
  }

  delete f;
  return;
}

inline bool IterativeScheduler::graphs_alive (SmacqGraph *f) {
  if (! (f->status & SMACQ_FREE)) {
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
  //fprintf(stderr, "shutdown started for %p\n", f);
  q.runable(f, NULL, SHUTDOWN);
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

  		do_shutdown(f);
	}

	return pretval;
}
    
/* Return 1 iff the datum was passed */
inline bool IterativeScheduler::run_consume(SmacqGraph * f, DtsObject d) {
        int outchan = 0;
	smacq_result retval;
	int status = false;

	//fprintf(stderr, "consume %p by %s (%p)\n", d.get(), f->name, f);

	//assert(! (f->status & SMACQ_FREE));
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
 	  do_shutdown(f);
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
  
  if (!f) {
    /* Datum fell off end of data-flow graph */
    //fprintf(stderr, "IterativeScheduler::element returning %p\n", d.get());
    
    dout = d;
    return SMACQ_PASS;
  } else if (! (f->status & SMACQ_FREE)) {
    switch (action) {

    case CONSUME:
      //fprintf(stderr, "sched_iterative_element calling consume on %p\n", &d);
      run_consume(f, d);
      break;
      
    case SHUTDOWN:
	if (q.pending(f)) {
		/* Defer last call until everything produced and consumed */
		q.runable(f, NULL, SHUTDOWN);
	} else {
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

/// Take an input and run it through a graph.
/// The graph may filter or modify it, but it shouldn't reorder things.
/// Return SMACQ_PASS or SMACQ_FREE.
inline smacq_result IterativeScheduler::decide(DtsObject din) {
  DtsObject dout;
  smacq_result r;
	smacq_result prev = SMACQ_NONE;

  input(din);

	// Run until nothing else we can do
	for (;;) {
		r = element(dout);

		if (r == SMACQ_NONE) {
			// Done
			break;
		}

		// Save this result for future
		prev = r;		
	}

	// Use next to last result as final word
	if (prev & SMACQ_PASS) {
  	return SMACQ_PASS;
	} else {
	  return SMACQ_FREE;
	}

}
#endif
