#include <stdlib.h>
#include <smacq-dataflow.h>
#include <DtsObject.h>
#include <smacq.h>

enum action { PRODUCE=1, SHUTDOWN=2, LASTCALL=3, CONSUME=4 };

struct qel {
  enum action action;
  smacq_graph * f;
  DtsObject d;
  struct qel * next;
#ifdef SMACQ_OPT_RUNRING
  struct qel * prev;
#endif
};
 	
struct runq {
  struct qel * head;
  struct qel * tail;
#ifndef SMACQ_OPT_RUNRING
  int empty;
#endif
};

#ifdef SMACQ_OPT_RUNRING

/* Use a ring of runq elements.  New elements are allocated as necesssary, but
 * we never shrink.  Most of the time we can reuse existing elements. 
 */

static inline struct qel * insert_before_head(struct runq * runq) {
	/* Insert new element before head */
 	struct qel * entry = (struct qel *)malloc(sizeof(struct qel));

	entry->prev = runq->head->prev;
	entry->prev->next = entry;

  	entry->next = runq->head;
	runq->head->prev = entry;

	return entry;
} 

void inline runable(struct runq * runq, smacq_graph * f, DtsObject d, enum action action) {
	struct qel * el;

	if ((action == PRODUCE) && runq->head) {
		/* runq->head->prev is guaranteed to be empty */
		el = runq->head->prev;

		/* but if it's also the tail, then we need a new element */
		if (el == runq->tail) {
			el = insert_before_head(runq);
		}
		
		runq->head = el;

	} else {
		el = runq->tail;
		runq->tail = runq->tail->next;

		/* if the new tail would stomp on the head, then make some more room */
		if (runq->tail == runq->head) {
			runq->tail = insert_before_head(runq);
		}
	}
			
	el->f = f;
	el->d = d;
	el->action = action;
		
	//fprintf(stderr, "%p now runable in %p/%p\n", runq->tail->d, runq, runq->tail);
	
	if (!runq->head) {
		runq->head = el;
	}
}

static inline int pop_runable(struct runq * runq, smacq_graph ** f, DtsObject &d, enum action * action) {
	if (!runq->head) {
		//fprintf(stderr, "queue %p/%p empty\n", runq, runq->head);
		return 0;
	}

	*f = runq->head->f;
	*action = runq->head->action;
	d = runq->head->d;

	//fprintf(stderr, "%p for %p off queue from %p/%p\n", runq->head->d, runq->head->f, runq, runq->head);

	runq->head->f = NULL;
	runq->head->d = NULL;

	runq->head = runq->head->next;

	if (runq->head == runq->tail) {
		/* Ring is empty */
		runq->head = NULL;
	}
	return 1;
}

static void init_runq(struct runq ** runqp) {
	struct runq * runq = (struct runq*)malloc(sizeof(struct runq));
	*runqp = runq;

	/* Make a 2 element ring to start */
	runq->tail = (struct qel*)malloc(sizeof(struct qel));
	runq->tail->next = (struct qel*)malloc(sizeof(struct qel));
	runq->tail->next->next = runq->tail;

	runq->tail->next->prev = runq->tail;
	runq->tail->prev = runq->tail->next;

	runq->head = NULL;
}

static inline int runq_empty(struct runq * runq) {
	if (!runq || !runq->head) {
		return 1;
	} else {
		return 0;
	}
}

#else
#warning "SMACQ_OPT_RUNRING not set"

/*
 * Use a list of runq elements.  New elements are allocated as necesssary, and 
 * freed after use.  This results in a lot of inefficient malloc() calls.
 */

void inline runable(struct runq * runq, smacq_graph * f, DtsObject d, enum action action) {
	struct qel * entry = malloc(sizeof(struct qel));
	entry->d = d;
	entry->f = f;
        entry->action = action;
	entry->next = NULL;

	if (action == PRODUCE) {
		entry->next = runq->head;
		runq->head = entry;
		if (!runq->tail) 
			runq->tail = entry;
	} else {
		if (runq->tail) {
			runq->tail->next = entry;
		}
		if (!runq->head) {
			runq->head = entry;
		}
		runq->tail = entry;
	}
}

static inline int pop_runable(struct runq * runq, smacq_graph ** f, DtsObject &d, enum action * action) {
	struct qel * res = runq->head;
	
	if (res) {
		*f = runq->head->f;
		*action = runq->head->action;
		d = runq->head->d;

		if (runq->tail == runq->head)
			runq->tail = NULL;

		runq->head = runq->head->next;

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

void queue_children(struct runq * runq, smacq_graph * f, DtsObject d, int outchan) {
  if (outchan >= 0 && f->numchildren) {
    assert(outchan < f->numchildren);
    runable(runq, f->child[outchan], d, CONSUME);
  } else if (f->numchildren) {
    int i;

    for (i=0; i < f->numchildren; i++) {
      if (f->child[i]) {
	runable(runq, f->child[i], d, CONSUME);
      }
    }
  } else {
    runable(runq, NULL, d, CONSUME);
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
  smacq_sched_iterative_shutdown(f, runq);
}

static int pending_normal(struct runq * runq, smacq_graph * f) {
	struct qel * q = runq->head;
	while (q) {
		if ((q->action != SHUTDOWN) && (q->action != LASTCALL) && (q->f == f)) {
			return 1;
		}
		q = q->next;
#ifdef SMACQ_OPT_RUNRING
		if (q == runq->tail) return 0;
#endif
	}
	return 0;
}

static int pending(struct runq * runq, smacq_graph * f) {
	struct qel * q = runq->head;
	while (q) {
		if ((q->action != SHUTDOWN) && (q->f == f)) {
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

  if (f->status) {
  //if (f->status & SMACQ_FREE) {
	/* Already shutdown, so do nothing */
	return;
  }

  delete f->instance;
  f->instance = NULL;
  f->status = SMACQ_FREE;

  /* Propagate to children */
  for(i=0; i < f->numchildren; i++) {
    /* Enqueue a terminate record */
    if (! (f->child[i]->status & SMACQ_FREE)) {
      //fprintf(stderr, "%p shutting down child %p\n", f, f->child[i]);
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

  if (! (f->status & SMACQ_FREE)) {
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
  runable(runq, f, NULL, LASTCALL);
  runable(runq, f, NULL, SHUTDOWN);
}

static inline smacq_result run_produce(smacq_graph * f, struct runq * runq) {
	DtsObject d = NULL;
	int outchan = -1;

	smacq_result pretval = f->instance->produce(d, &outchan);

	if (pretval & SMACQ_PASS) {
		queue_children(runq, f, d, outchan);
		
	}

	if (pretval & (SMACQ_PRODUCE|SMACQ_CANPRODUCE)) {
		/* Come back for more */
		runable(runq, f, NULL, PRODUCE);
	}

	if (pretval & SMACQ_END) {
		assert(!(pretval & (SMACQ_PRODUCE|SMACQ_CANPRODUCE)));

  		do_shutdown(runq, f);
	}

	return pretval;
}
    
/* Return 1 iff the datum was passed */
static inline int run_consume(smacq_graph * f, DtsObject d, struct runq * runq) {
        int outchan = -1;
	smacq_result retval;
	int status = 0;

	//assert(! (f->status & SMACQ_FREE));
	retval = f->instance->consume(d, &outchan);

	if (retval & SMACQ_PASS) {
	  //fprintf(stderr, "Pass on %p by %p\n", d, f);
	  queue_children(runq, f, d, outchan);
	  status = 1;
	}
	

        if (retval & (SMACQ_PRODUCE|SMACQ_CANPRODUCE)) {
	      run_produce(f, runq);
        }

        if (retval & SMACQ_END) {
 	  do_shutdown(runq, f);
        }

	return status;
}

void smacq_sched_iterative_input(smacq_graph * startf, DtsObject din, struct runq * runq) {
  if (din) {
    runable(runq, startf, din, CONSUME);
  }
}

void smacq_sched_iterative_init(smacq_graph * startf, struct runq ** runqp, int produce_first) {
  struct runq * runq = *runqp;

     /* First call */
     init_runq(runqp);
     runq = *runqp;

     if (produce_first) {
	  while(startf) {
		  //fprintf(stderr, "produce_first for %p\n", startf);
        	  /* Force first guy to produce */
		  runable(runq, startf, NULL, PRODUCE);
		  startf = startf->next_graph;
	  }
     }
}

/* Handle one thing on the run queue */
static inline smacq_result smacq_sched_iterative_element(DtsObject &dout, struct runq * runq) {
      smacq_graph * f;
      DtsObject d;
      enum action action;

      if (!pop_runable(runq, &f, d, &action)) return SMACQ_ERROR;

      if (!f) {
	/* Datum fell off end of data-flow graph */
	//fprintf(stderr, "sched_iterative_element returning %p\n", d);

	if (dout) {
	  dout = d;
	  return SMACQ_PASS;
	} else {
	  
	  return SMACQ_FREE;
	}
      } else if (! (f->status & SMACQ_FREE)) {
		switch (action) {
			case CONSUME:
				//fprintf(stderr, "sched_iterative_element calling consume on %p\n", d);
				run_consume(f, d, runq);
				
			case SHUTDOWN:
				if (pending(runq,f)) {
					/* Defer last call until everything produced and consumed */
					runable(runq, f, NULL, SHUTDOWN);
				} else {
					do_shutdown(runq, f);
				}
				break;
			case LASTCALL:
				if (pending_normal(runq,f)) {
					/* Defer last call until everything produced and consumed */
					runable(runq, f, NULL, LASTCALL);
				} else {
					run_produce(f, runq);
				}
				break;
			case PRODUCE:
				run_produce(f, runq);
				break;
		}
      }

      return SMACQ_FREE;
}

static inline smacq_result smacq_sched_iterative_once(smacq_graph * startf, DtsObject * dout, struct runq * runq, int produce_first) {
  *dout = NULL;

  smacq_result r = smacq_sched_iterative_element(*dout, runq);

  /* Run until something falls off the edge, or until the queue is empty */
  if (r != SMACQ_ERROR) {
	return r;
  } else if (produce_first || (!smacq_sched_iterative_graph_alive(startf)) ) {
    	return SMACQ_END;
  } else {
	return SMACQ_FREE;
  }
}

smacq_result smacq_sched_iterative_busy(smacq_graph * startf, DtsObject * dout, struct runq * runq, int produce_first) {
  *dout = NULL;

  while ((SMACQ_ERROR != smacq_sched_iterative_element(*dout, runq)) && (! *dout)) {}

  return (smacq_result)0;
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
smacq_result smacq_sched_iterative(smacq_graph * startf, DtsObject din, DtsObject *dout, struct runq ** runqp, int produce_first) {
  	if (!(*runqp)) 	smacq_sched_iterative_init(startf, runqp, produce_first);
	if (din) 	smacq_sched_iterative_input(startf, din, *runqp);
	return smacq_sched_iterative_once(startf, dout, *runqp, produce_first);
}


