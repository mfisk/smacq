#ifndef SMACQ_SCHEDULER_CPP
#define SMACQ_SCHEDULER_CPP
#include <SmacqScheduler.h>
#include <SmacqGraph.h>
#include <ThreadSafe.h>
#include <pthread.h>
#include <iostream>

#include <boost/lambda/bind.hpp>

#ifndef METHOD
#define METHOD 
#endif

METHOD void SmacqScheduler::thread_loop() {
	for (;;) {
		// As long as we can do things, do it
		while (do_something()) ; 
		
		// Now, there might be stuff to do, but those module(s) are already locked,
		// or a running module may be putting something on the queue
		// or it might be that nobody is doing anything and we're done.

		Idling.increment();
  		if (debug) fprintf(stderr, "** Thread %p idling\n", (void*)pthread_self());

		// We don't atomically check both queues, but if they're changing then I don't think
		// everybody is idling.
		while (consumeq.empty() && produceq.empty()) {
			struct timespec request = {0, 1000};
			struct timespec remain;

			if (Idling.get() == (int)threads.size()) {
  				if (debug) fprintf(stderr, "** Thread %p exiting\n", (void*)pthread_self());
				// yes, we want to return without decrementing Idling, else other processes won't detect us as idle and exit themselves
				return;
			}

			nanosleep(&request, &remain);

			// Exponential backoff
			if (request.tv_nsec >= 500000000L) {
				request.tv_sec = 1; 
				request.tv_nsec = 0;
			} else {
				request.tv_sec *= 2;
				request.tv_nsec *= 2;
			}
		}
		Idling.decrement();
	}
}
  
METHOD void SmacqScheduler::start_threads(int numt) {
  assert(threads.size() == 0);
  threads.resize(numt+1);
  if (!numt) return;

  if (debug) fprintf(stderr, "** Starting %d slave threads\n", numt);

  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  threads[0] = pthread_self();
  for (int i = 1; i <= numt; i++) {
        assert(!pthread_create(&threads[i], &attr, iterative_scheduler_thread_start, this));
  }

  pthread_attr_destroy(&attr);
}

METHOD void SmacqScheduler::join_threads() {
  if (!threads.size()) return;
  if (debug) fprintf(stderr, "** Waiting for slave threads to exit\n");

  // Make sure they know we're idle
  Idling.increment();

  for (unsigned int i = 1; i < threads.size(); i++) {
	pthread_join(threads[i], NULL);
  }

  threads.clear();
  //Idling.set(0); // in case scheduler reused
}

METHOD void SmacqScheduler::seed_produce_one(SmacqGraphNode * g) {
    using namespace boost::lambda;
    using namespace std;

    // We should be a stub and should pass this to children.
    if (!g->argc) {
	FOREACH_CHILD(g, produceq.enqueue(child) );
    } else {
	g->seed_produce();
    }
}

METHOD void SmacqScheduler::seed_produce(SmacqGraph * startf) {
  using namespace boost::lambda;

  // Force first guy to produce
  startf->head.foreach( bind(&SmacqScheduler::seed_produce_one, this, DEREF(_1)) );
}

METHOD void SmacqScheduler::input_one(SmacqGraphNode * g, DtsObject din) {
    if (g->argc) {
    	assert (g->instance > (void*)1000);
    	g->runable(din);
    } else {
	// stub
	FOREACH_CHILD(g, child->runable(din));
    }
}

METHOD void SmacqScheduler::input(SmacqGraph * c, DtsObject din) {
  using namespace boost::lambda;
  c->head.foreach( bind(&SmacqScheduler::input_one, this, DEREF(_1), din) );
}

METHOD void SmacqScheduler::enqueue(SmacqGraphNode * caller, DtsObject d, int outchan) {
  queue_children(caller, d, outchan);

  // If the consumeq has as many modules as there are threads, 
  // then we should try to go process it.
  while ((consumeq.size() > threads.size())) {
	if (debug) caller->log("enqueue() yielding to do_something()");
	if (!do_something(true)) break;
  }
}

METHOD void SmacqScheduler::queue_children(SmacqGraphNode_ptr f, DtsObject d, int outchan) {
  using namespace boost::lambda;

  //fprintf(stderr, "Output channel was %d of %u\n", outchan, f->children.size());
  assert((outchan < (int)f->children.size()) || (!f->children.size() && !outchan));

  assert (f->instance > (void*)1000);

  if (f->children[outchan].size()) {
    if (debug) f->log("output %p to children", d.get());
    f->children[outchan].foreach( bind(&SmacqGraphNode::runable, DEREF(_1), d) );
  } else {
    if (debug) f->log("output %p to outputq", d.get());
    outputq.enqueue(d);
  }
}


/// Graph must already be locked
METHOD void SmacqScheduler::run_produce(SmacqGraphNode_ptr f) {
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
      SmacqGraphNode::do_shutdown(f);

    } else if (pretval & (SMACQ_PRODUCE|SMACQ_CANPRODUCE)) {
      f->seed_produce(); // Keep producing

    } else {
      f->produce_done();
    }
  }
}

/// Try to consume something.    
/// Return true iff progress was made.
METHOD bool SmacqScheduler::run_consume(SmacqGraphNode_ptr i) {
  if (!i->mustProduce.get()   // This will automatically get rescheduled after the produce
     && !i->shutdown.get()) { // Or already shutdown
  
 	assert(i->instance);

  	DtsObject d;

  	// We're a little greedy here in order to create more locality.
  	// We process all the pending inputs for this module.
  	while (!i->mustProduce.get() && i->inputq.pop(d)) {
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
      			SmacqGraphNode::do_shutdown(i);
			break;
    		}
	}
  }

  return true;
}

/// Returns a locked graph from the queue, or nothing at all
METHOD SmacqGraphNode_ptr SmacqScheduler::pop_lock(runq<SmacqGraphNode_ptr> & q) {
  SmacqGraphNode_ptr nullp;
  SmacqGraphNode_ptr g;
  RecursiveLock l(q);

  if (q.peek(g) && g->try_lock()) {
     SmacqGraphNode_ptr newg; 
     q.pop(newg);

     // Make sure we popped the same thing we peeked.
     assert(g == newg);
     return g;
  } else {

     return nullp;
  }
}

METHOD bool SmacqScheduler::done() {
  int retval;

  //std::cout << "before lock 1 closes\n";
  RecursiveLock l1(consumeq);
  //std::cout << "after lock 1 closes and before lock 2 closes\n";
  RecursiveLock l3(produceq);
  //std::cout << "after lock 2 closes\n";
 
  retval = consumeq.empty();
  //std::cout << "after consumeq.empty" << retval << '\n';
  retval = retval && produceq.empty();
  //std::cout << "after produceq.empty" << retval << '\n';
  retval = retval && Idling.get();
  //std::cout << "after Idling.get" << retval << '\n';
  retval = retval == ((int) threads.size() - 1);
  //std::cout << "after threads.size" << retval <<  '\n';

  return(consumeq.empty() && produceq.empty() && Idling.get() == ((int)threads.size() - 1));
}

/// Return true iff we had something to do.
METHOD bool SmacqScheduler::do_something(bool consume_only) {
  SmacqGraphNode_ptr f;

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

/// Do a _little_ work.
/// dout will be set to NULL or to a returned object.
/// Returns false if we're done and true if we want to be called again.
METHOD bool SmacqScheduler::element(DtsObject &dout) {
  dout = NULL;
  do_something();
  outputq.pop(dout);
  if (done() && outputq.empty()) {
	return false;
  } else {
	// Come back for more
	return true;
  }
}

METHOD DtsObject SmacqScheduler::pyelement() {
  DtsObject dout = NULL;
  do_something();
  outputq.pop(dout);
  return dout;
}

/// Process until completion.  Returns true unless there is an error.  Ignores any output.
METHOD bool SmacqScheduler::busy_loop() {
  DtsObject d;
  //std::cout << "Starting (inside) busy_loop.\n";
  while (get()) {;}

  return true;
}

METHOD DtsObject SmacqScheduler::get() {
  DtsObject d;
  for (;;) {
    do_something();
    if (outputq.pop(d)) return d;
    if (done()) return NULL;
  }
}


METHOD bool SmacqScheduler::decide_one(SmacqGraphNode * g, DtsObject din) {
	return (decide(g, din) == SMACQ_PASS);
}

METHOD smacq_result SmacqScheduler::decide_set(ThreadSafeMultiSet<SmacqGraphNode_ptr> & g, DtsObject din) {
  using namespace boost::lambda;

  if (g.has_if( bind<smacq_result>(&SmacqScheduler::decide_one, this, DEREF(_1), din))) {
	return SMACQ_PASS;
  }
  return SMACQ_FREE;
}

METHOD smacq_result SmacqScheduler::decide_children(SmacqGraphNode * g, DtsObject din, int outchan) {
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
METHOD smacq_result SmacqScheduler::decideContainer(SmacqGraph * g, DtsObject din) {
  return decide_set(g->head, din);
}

/// Take an input and run it through a boolean graph.
/// Return SMACQ_PASS or SMACQ_FREE.
METHOD smacq_result SmacqScheduler::decide(SmacqGraphNode * g, DtsObject din) {
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
