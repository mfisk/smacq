#include <smacq.h>
#include <stdlib.h>

/*
 * Note:  This scheduler uses the "status" element of the smacq_graph structure. 
 * Normally it is unset, but when a module is politely asked to shutdown,
 * it is set to SMACQ_END|SMACQ_PRODUCE and a produce trigger is enqueue.
 * It will stay in this state ("last call") until the produce method fails to
 * return new data.  At that point, it will go to just SMACQ_END and do_shutdown()
 * will be called.
 *
 * When do_shutdown() is called, it checks that no consumes are pending for the module.
 * If so, then it enqueues a produce for later.  Otherwise it calls the shutdown method
 * and sets the state to SMACQ_END|SMACQ_FREE at which point the module cannot be used 
 * again.  The produce handler will call do_shutdown() again whenver it gets a 
 * SMACQ_END without SMACQ_PRODUCE.
 */


struct qel {
  smacq_graph * f;
  const dts_object * d;
  struct qel * next;
};
 	
struct runq {
  struct qel * head;
  struct qel * tail;
  int empty;
  //int size;
};

#ifdef SMACQ_OPT_RUNRING

/* Use a ring of runq elements.  New elements are allocated as necesssary, but
 * we never shrink.  Most of the time we can reuse existing elements. 
 */

void inline runable(struct runq * runq, smacq_graph * f, const dts_object * d) {
	runq->tail->f = f;
	runq->tail->d = d;
        if (d) dts_incref(d, 1);

	runq->empty = 0;

	// fprintf(stderr, "%p now runable, ", runq->tail);

	if (runq->tail->next == runq->head) {
		/* Insert new element before head */
 		struct qel * entry = malloc(sizeof(struct qel));
  		entry->next = runq->head;
		runq->tail->next = entry;
	} 
	runq->tail = runq->tail->next;

	// fprintf(stderr, "%p will be next\n", runq->tail);
}
		
static inline int pop_runable(struct runq * runq, smacq_graph ** f, const dts_object **d) {
	struct qel * res = runq->head;

	if (runq->empty)
		return 0;

	*f = res->f;
	*d = res->d;

	runq->head = runq->head->next;

	// fprintf(stderr, "%p off queue\n", res);
	if (runq->head == runq->tail) {
		/* Ring is empty */
		runq->empty = 1;
	}
	return 1;
}

static void init_runq(struct runq ** runqp) {
	struct runq * runq = malloc(sizeof(struct runq));
	*runqp = runq;

	/* Make a 2 element ring to start */
	runq->head = malloc(sizeof(struct qel));
	runq->head->next = malloc(sizeof(struct qel));
	runq->head->next->next = runq->head;
	runq->tail = runq->head;

	runq->empty = 1;
}

static inline int runq_empty(struct runq * runq) {
	if (!runq || runq->empty) {
		return 1;
	} else {
		return 0;
	}
}

#warning "SMACQ_OPT_RUNRING not set"
#else

/*
 * Use a list of runq elements.  New elements are allocated as necesssary, and 
 * freed after use.  This results in a lot of inefficient malloc() calls.
 */

void inline runable(struct runq * runq, smacq_graph * f, const dts_object * d) {
	struct qel * entry = malloc(sizeof(struct qel));
	entry->d = d;
	entry->f = f;
	entry->next = NULL;

        if (d) dts_incref(d, 1);

	if (runq->tail) {
		runq->tail->next = entry;
	}
	if (!runq->head) {
		runq->head = entry;
	}
	runq->tail = entry;
}

static inline int pop_runable(struct runq * runq, smacq_graph ** f, const dts_object **d) {
	struct qel * res = runq->head;
	
	if (res) {
		runq->head = res->next;
		*f = res->f;
		*d = res->d;

		if (runq->tail == res)
			runq->tail = NULL;

		free(res);
		return 1;
	} else {
		return 0;
	}
}

static void init_runq(struct runq ** runqp) {
	struct runq * runq = malloc(sizeof(struct runq));
	*runqp = runq;

	runq->head = NULL;
	runq->tail = NULL;
}

static inline int runq_empty(struct runq * runq) {
	if (!runq || !runq->head) {
		return 1;
	} else {
		return 0;
	}
}

#endif

void queue_children(struct runq * runq, smacq_graph * f, const dts_object * d, int outchan) {
  if (outchan >= 0) {
    assert(outchan < f->numchildren);
    runable(runq, f->child[outchan], d);
  } else {
    int i;

    for (i=0; i < f->numchildren; i++) {
      if (f->child[i]) {
	runable(runq, f->child[i], d);
      }
    }
  }
}


void do_shutdown(struct runq * runq, smacq_graph *);

void check_for_shutdown(struct runq * runq, smacq_graph *f) {
  int i;
  for (i=0; i < f->numchildren; i++) {
    if (f->child[i] && (! (f->child[i]->status & SMACQ_FREE))) 
      return;
  }

  /* No more active children.  Clean-up self and parents. */
  for (i=0; i < f->numparents; i++) {
    if (f->parent[i] && (! (f->parent[i]->status & SMACQ_FREE))) 
      smacq_sched_iterative_shutdown(f->parent[i], runq);
  }
  do_shutdown(runq, f);
}

static int pending(struct runq * runq, smacq_graph * f) {
	struct qel * q = runq->head;
	while (q) {
		if (q->d && (q->f == f)) {
			return 1;
		}
		q = q->next;
#ifdef SMACQ_OPT_RUNRING
		if (q == runq->tail) return 0;
#endif
	}
	return 0;
}

void do_shutdown(struct runq * runq, smacq_graph *f) {
  int i;

  if (f->status & SMACQ_FREE) {
	/* Already shutdown, so do nothing */
	return;
  }
  if (pending(runq, f)) {
	/* Defer */
	runable(runq, f, NULL);
	return;
  }

  fprintf(stderr, "shutting down %p %s with state %p\n", f, f->name, f->state);

  if (f->ops.shutdown) {
    f->ops.shutdown(f->state);
  }
  f->state = NULL;  /* Just in case somebody tries to use it */
  f->status = SMACQ_END|SMACQ_FREE;

  /* Propagate to children */
  for (i=0; i < f->numchildren; i++) {
    /* Enqueue a terminate record */
    if (! (f->child[i]->status & SMACQ_END)) {
      smacq_sched_iterative_shutdown(f->child[i], runq);
    }
  }

  /* Propagate to parents */
  for (i=0; i< f->numparents; i++) {
    check_for_shutdown(runq, f->parent[i]);
  }

  return;
}

static int smacq_sched_iterative_graph_alive (smacq_graph *f) {
  int i;

  if (! (f->status & SMACQ_END)) {
    return 1;
  }

  for (i=0; i<f->numchildren; i++) {
    if (f->child[i]) {
      if (smacq_sched_iterative_graph_alive(f->child[i])) {
	return 1;
      }
    }
  }

  return 0;
}

void smacq_sched_iterative_shutdown(smacq_graph * f, struct runq * runq) {
  f->status |= (SMACQ_END|SMACQ_PRODUCE);

  runable(runq, f, NULL);
}

static inline void run_produce(smacq_graph * f, struct runq * runq) {
	const dts_object * d = NULL;
	int outchan = -1;

	if (f->status & SMACQ_END) {
		/* Defer last call until everything consumed */
		if (pending(runq,f)) {
			runable(runq, f, NULL);
			return;
		}

		if (! (f->status & SMACQ_PRODUCE)) {
			do_shutdown(runq, f);
			return;
		}
	}

        if (! (f->status & SMACQ_FREE)) {
		int pretval = f->ops.produce(f->state, &d, &outchan);

		if (pretval & SMACQ_PASS) {
			queue_children(runq, f, d, outchan);
			dts_decref(d);
		}

		if (pretval & (SMACQ_PRODUCE|SMACQ_CANPRODUCE)) {
			/* Come back for more */
			runable(runq, f, NULL);
		}

		if (f->status & SMACQ_END) {
			if (! (pretval & (SMACQ_PASS|SMACQ_PRODUCE|SMACQ_CANPRODUCE))) {
				/* Produced nothing; last call is over */
				f->status = SMACQ_END;
				pretval |= SMACQ_END;
				//fprintf(stderr, "%s:%p produced nothing while ending\n", f->name, f);
			}
		}

		if (pretval & SMACQ_END) {
			assert(!(pretval & (SMACQ_PRODUCE|SMACQ_CANPRODUCE)));

	  		do_shutdown(runq, f);
		}
	}
}
      
static inline void run_consume(smacq_graph * f, const dts_object * d, struct runq * runq) {
        int outchan = -1;
	smacq_result retval;

	if (f->status & SMACQ_FREE) {
		fprintf(stderr, "Consume called after module %s (%p) freed\n", f->name, f);
		return;
	}

	//assert(! (f->status & SMACQ_FREE));
	retval = f->ops.consume(f->state, d, &outchan);

	if (retval & SMACQ_PASS) {
	  //fprintf(stderr, "Pass on %p by %p\n", d, f);
	  queue_children(runq, f, d, outchan);
	}
	dts_decref(d);

        if (retval & (SMACQ_PRODUCE|SMACQ_CANPRODUCE)) {
	      run_produce(f, runq);
        }

        if (retval & SMACQ_END) {
 	  do_shutdown(runq, f);
        }
}


/*
 * If dout is non-NULL, when an object is passed or produced by a leaf node, this function will return it in dout.
 * The runq should point to NULL initially, and should be treated opaquely by the caller.
 * If din is specified, then that object will passed to startf's consume() call.
 * If din is NULL, then startf's produce() call will be used, if produce_first is true.
 * *record will be NULL if no data was produced.
 *
 * Return values:
 *      SMACQ_FREE  -  no data was produced
 *      SMACQ_PASS  -  dout was set
 *      SMACQ_END  -  do not call again
 */
int smacq_sched_iterative(smacq_graph * startf, const dts_object * din, const dts_object ** dout, struct runq ** runqp, int produce_first) {
  struct runq * runq = *runqp;
  smacq_graph * f;
  const dts_object * d;
  *dout = NULL;

  if (!runq) {
     /* First call */
     init_runq(runqp);
     runq = *runqp;

     if (produce_first) {
	  while(startf) {
		  //fprintf(stderr, "produce_first for %p\n", startf);
        	  /* Force first guy to produce */
		  runable(runq, startf, NULL);
		  startf = startf->next_graph;
	  }
     }
  }

  if (din) {
    runable(runq, startf, din);
  }

  if (pop_runable(runq, &f, &d)) {
      if (!f) {
	/* Datum fell off end of data-flow graph */

	if (dout) {
	  *dout = d;
	  return SMACQ_PASS;
	} else {
	  dts_decref(d);
	  return SMACQ_FREE;
	}
      } 
      
      if (d) {
	run_consume(f, d, runq);
      } else {
	/* Force produce */
	run_produce(f, runq);
      }
  } else if (produce_first || (!smacq_sched_iterative_graph_alive(startf)) ) {
    return SMACQ_END;
  }

  return SMACQ_FREE;
}


