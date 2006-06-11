#ifndef SMACQ_SCHEDULER_CPP
#define SMACQ_SCHEDULER_CPP
#include <SmacqScheduler.h>
#include <SmacqGraph.h>
#include <ThreadSafe.h>
#include <pthread.h>

#include <boost/lambda/bind.hpp>

#ifndef METHOD
#define METHOD 
#endif

METHOD void SmacqScheduler::thread_loop() {
	for (;;) {
		// As long as we can do things, do it
		while (do_something()) ; 

		if (done()) return;

		// XXX: should wait for a condition variable
		//pthread_cond_wait(&todo, lock);
	}
}
  
METHOD void SmacqScheduler::slave_threads(int numt) {
  assert(threads.size() == 0);

  threads.resize(numt);

  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  for (int i = 0; i < numt; i++) {
        assert(!pthread_create(&threads[i], &attr, iterative_scheduler_thread_start, this));
  }

  pthread_attr_destroy(&attr);
}

METHOD void SmacqScheduler::join_threads() {
  for (unsigned int i = 0; i < threads.size(); i++) {
	pthread_join(threads[i], NULL);
  }

  threads.clear();
}

METHOD void SmacqScheduler::seed_produce_one(SmacqGraph * g) {
    using namespace boost::lambda;
    using namespace std;

    // We should be a stub and should pass this to children.
    if (!g->argc) {
	FOREACH_CHILD(g, produceq.enqueue(child) );
    } else {
	g->seed_produce();
    }
}

METHOD void SmacqScheduler::seed_produce(SmacqGraphContainer * startf) {
  using namespace boost::lambda;

  // Force first guy to produce
  startf->head.foreach( bind(&SmacqScheduler::seed_produce_one, this, DEREF(_1)) );
}

METHOD void SmacqScheduler::input_one(SmacqGraph * g, DtsObject din) {
    if (g->argc) {
    	assert (g->instance > (void*)1000);
    	g->runable(din);
    } else {
	// stub
	FOREACH_CHILD(g, child->runable(din));
    }
}

METHOD void SmacqScheduler::input(SmacqGraphContainer * c, DtsObject din) {
  using namespace boost::lambda;
  c->head.foreach( bind(&SmacqScheduler::input_one, this, DEREF(_1), din) );
}

METHOD void SmacqScheduler::enqueue(SmacqGraph * caller, DtsObject d, int outchan) {
  queue_children(caller, d, outchan);

  // If the consumeq has as many modules as there are threads, 
  // then we should try to go process it.
  while ((consumeq.size() > threads.size()) && do_something(true)) { }
}

METHOD void SmacqScheduler::queue_children(SmacqGraph_ptr f, DtsObject d, int outchan) {
  using namespace boost::lambda;

  //fprintf(stderr, "Output channel was %d of %u\n", outchan, f->children.size());
  assert((outchan < (int)f->children.size()) || (!f->children.size() && !outchan));

  assert (f->instance > (void*)1000);

  if (f->children[outchan].size()) {
    if (debug) f->log("output %p to children", d.get());
    f->children[outchan].foreach( bind(&SmacqGraph::runable, DEREF(_1), d) );
  } else {
    if (debug) f->log("output %p to outputq", d.get());
    outputq.enqueue(d);
  }
}


/// Graph must already be locked
METHOD void SmacqScheduler::run_produce(SmacqGraph_ptr f) {
  int outchan = 0;
  DtsObject d = NULL;
  smacq_result pretval;

  // So here's the problem:  If run_produce returns SMACQ_PRODUCE, then we 
  // cannot run consume on this module until we have finished producing.
  // But we don't want to produce too eagerly before letting other modules
  // consume (or else we may buffer too much stuff in memory).

  if (!f->shutdown.get()) {
    assert(f->instance);
    pretval = f->instance->produce(d, outchan);

    if (pretval & SMACQ_PASS) {
      queue_children(f, d, outchan);		
    }

    if (pretval & SMACQ_END) {
      //assert(!(pretval & (SMACQ_PRODUCE|SMACQ_CANPRODUCE)));
 
      if (debug) f->log("asked for shutdown on produce"); 
      SmacqGraph::do_shutdown(f);

    } else if (pretval & (SMACQ_PRODUCE|SMACQ_CANPRODUCE)) {
      f->seed_produce(); // Keep producing

    } else {
      f->produce_done();
    }
  }
}

/// Try to consume something.    
/// Return true iff progress was made.
METHOD bool SmacqScheduler::run_consume(SmacqGraph_ptr i) {
  if (!i->mustProduce   // This will automatically get rescheduled after the produce
     && !i->shutdown.get()) { // Or already shutdown
  
 	assert(i->instance);

  	DtsObject d;

  	// We're a little greedy here in order to create more locality.
  	// We process all the pending inputs for this module.
  	while (!i->mustProduce && i->inputq.pop(d)) {
  		if (debug) i->log("consume(%p)", d.get()); 

    		int outchan = 0;
    		smacq_result retval = i->instance->consume(d, outchan);
	
    		if (retval & SMACQ_PASS) {
      			//fprintf(stderr, "Pass on %p to chan %d by %p\n", d.get(), outchan, i.get());
      			queue_children(i, d, outchan);
    		}
	
    		if (retval & (SMACQ_PRODUCE|SMACQ_CANPRODUCE)) {
			if (debug) i->log("consume triggered produce");
      			run_produce(i);
    		}
		
    		if (retval & SMACQ_END) {
      			//fprintf(stderr, "module %p asked for shutdown on consume\n", i.get()); 
      			SmacqGraph::do_shutdown(i);
    		}
	}
  }

  return true;
}

/// Returns a locked graph from the queue, or nothing at all
METHOD SmacqGraph_ptr SmacqScheduler::pop_lock(runq<SmacqGraph_ptr> & q) {
  SmacqGraph_ptr nullp;
  SmacqGraph_ptr g;
  RecursiveLock l(q);

  if (q.peek(g) && g->try_lock()) {
     SmacqGraph_ptr newg; 
     q.pop(newg);

     // Make sure we popped the same thing we peeked.
     assert(g == newg);
     return g;
  } else {

     return nullp;
  }
}

METHOD bool SmacqScheduler::done() {
  RecursiveLock l1(consumeq);
  RecursiveLock l3(produceq);

  return(consumeq.empty() && produceq.empty());
}

/// Return true iff we had something to do.
METHOD bool SmacqScheduler::do_something(bool consume_only) {
  SmacqGraph_ptr f;

  if ((f = pop_lock(consumeq))) {
    if (debug) f->log("run_consume()");
	
    run_consume(f);
    f->unlock();

  } else if (consume_only) {
    return false;

  } else if ((f = pop_lock(produceq))) {
    if (debug) f->log("run_produce");
    run_produce(f);
    f->unlock();

  } else {
    return false;
  }

  return true;
}

/// Handle one thing on the run queue and return something if possible.
/// Return SMACQ_PASS iff an object falls off the end of the graph.
/// Return SMACQ_NONE iff there is no work we can do.
/// Otherwise, return SMACQ_FREE.
METHOD smacq_result SmacqScheduler::element(DtsObject &dout) {
  if (!do_something()) {
    if (outputq.pop(dout)) {
      // Datum fell off end of data-flow graph
      return SMACQ_PASS;

    } else {
      // All done!
      return SMACQ_NONE;
    }
  }

  // See if we can return something too.
  if (outputq.pop(dout)) {
    return SMACQ_PASS;
  } else {
    return SMACQ_FREE;
  }
}

/// Process until completion.  Returns true unless there is an error.  Ignores any output.
METHOD bool SmacqScheduler::busy_loop() {
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

METHOD smacq_result SmacqScheduler::get(DtsObject &dout) {
  dout = NULL;
  smacq_result r;
  smacq_result done = SMACQ_ERROR|SMACQ_END|SMACQ_PASS;
  do {
    r = element(dout);
  } while (r != SMACQ_NONE && ! (r & done));

  return r;
}

METHOD bool SmacqScheduler::decide_one(SmacqGraph * g, DtsObject din) {
	return (decide(g, din) == SMACQ_PASS);
}

METHOD smacq_result SmacqScheduler::decide_set(ThreadSafeMultiSet<SmacqGraph_ptr> & g, DtsObject din) {
  using namespace boost::lambda;

  if (g.has_if( bind<smacq_result>(&SmacqScheduler::decide_one, this, DEREF(_1), din))) {
	return SMACQ_PASS;
  }
  return SMACQ_FREE;
}

METHOD smacq_result SmacqScheduler::decide_children(SmacqGraph * g, DtsObject din, int outchan) {
  if (!g->children[outchan].size()) {
      // Base case: got to an end of decision graph!
      return SMACQ_PASS;
  }

  assert(outchan < (int)g->children.size());

  // Simpler case if top node is a stub
  return decide_set(g->children[outchan], din);
}

/// Take an input and run it through a boolean graph.
/// Return SMACQ_PASS or SMACQ_FREE.
METHOD smacq_result SmacqScheduler::decideContainer(SmacqGraphContainer * g, DtsObject din) {
  return decide_set(g->head, din);
}

/// Take an input and run it through a boolean graph.
/// Return SMACQ_PASS or SMACQ_FREE.
METHOD smacq_result SmacqScheduler::decide(SmacqGraph * g, DtsObject din) {
  int outchan = 0;
  assert(g);

  if (!g->argc) return decide_children(g, din, outchan);

  assert(g->instance);
  smacq_result r = g->instance->consume(din, outchan);

  assert(! (r & (SMACQ_PRODUCE|SMACQ_CANPRODUCE|SMACQ_END)));

  if (r & SMACQ_PASS) {
    //fprintf(stderr, "Pass on %p by %p\n", d.get(), f);
    return decide_children(g, din, outchan);
  }
  return SMACQ_FREE;
}
#endif
