#include <smacq.h>
#include <stdlib.h>

/*
 * Note:  This scheduler uses the "status" element of the smacq_graph structure. 
 * Normally it is unset, but when a module's shutdown is pending, it is SMACQ_END.
 * After the shutdown, it is SMACQ_END|SMACQ_FREE and should no longer be used.
 */

struct runq {
  smacq_graph * f;
  const dts_object * d;
  struct runq * next;
  struct runq * tail;
};

void runable(struct runq ** runqp, smacq_graph * f, const dts_object * d) {
  struct runq * entry;

  if (f && (f->status & SMACQ_FREE)) return;

  //fprintf(stderr, "runable %p for  %p\n", d, f);
  assert(!d || dts_gettype(d));

  entry = malloc(sizeof(struct runq));
  entry->f = f;
  entry->d = d;
  entry->next = NULL;

  if (d) dts_incref(d, 1);

  if (*runqp) {
  	(*runqp)->tail->next = entry;
  } else {
	*runqp = entry;
  }
  (*runqp)->tail = entry;
}
  
static void pop_runable(struct runq ** runqp) {
  struct runq * runq = *runqp;
  assert(runq);
  //fprintf(stderr, "pop-run %p from %p\n", runq->d, runq->f);
  *runqp=runq->next;
  if (*runqp) {
	  (*runqp)->tail = runq->tail;
  }
  free(runq);
}

void queue_children(struct runq ** runq, smacq_graph * f, const dts_object * d, int outchan) {
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


void do_shutdown(struct runq ** runqp, smacq_graph *);

void check_for_shutdown(struct runq ** runqp, smacq_graph *f) {
  int i;
  for (i=0; i < f->numchildren; i++) {
    if (f->child[i] && (! (f->child[i]->status & SMACQ_FREE))) 
      return;
  }

  /* No more active children.  Clean-up self and parents. */
  for (i=0; i < f->numchildren; i++) {
    if (f->child[i] && (! (f->child[i]->status & SMACQ_FREE))) 
      smacq_sched_iterative_shutdown(f->child[i], runqp);
  }
  //do_shutdown(runqp, f);
}

void do_shutdown(struct runq ** runqp, smacq_graph *f) {
  int i;

  if (f->status & SMACQ_FREE) {
	  /* Already shutdown, so do nothing */
	  return;
  }

  //fprintf(stderr, "shutting down %p %s\n", f, f->name);

  if (f->ops.shutdown) {
    f->ops.shutdown(f->state);
  }
  f->state = NULL;  /* Just in case somebody tries to use it */

  // fprintf(stderr, "module %p %s ended\n", f, f->name);
  f->status = SMACQ_END|SMACQ_FREE;

  /* Propagate to children */
  for (i=0; i < f->numchildren; i++) {
    /* Enqueue a terminate record */
    if (! (f->child[i]->status & SMACQ_END)) {
      smacq_sched_iterative_shutdown(f->child[i], runqp);
    }
  }

  /* Propagate to parents */
  for (i=0; i< f->numparents; i++) {
    check_for_shutdown(runqp, f->parent[i]);
  }
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

void smacq_sched_iterative_shutdown(smacq_graph * f, struct runq ** runqp) {
  f->status |= SMACQ_END;

  runable(runqp, f, NULL);
}

static int pending(struct runq * runq, smacq_graph * f) {
	while (runq) {
		if (runq->d && (runq->f == f)) {
			return 1;
		}
		runq = runq->next;
	}
	return 0;
}

static int run_produce(smacq_graph * f, struct runq ** runqp, smacq_graph * startf) {
	const dts_object * d = NULL;
	int outchan = -1;
	int pretval;

        if (! (f->status & SMACQ_FREE)) {
		pretval = f->ops.produce(f->state, &d, &outchan);

		if (pretval & SMACQ_PASS) {
			queue_children(runqp, f, d, outchan);
			dts_decref(d);
		}

		if (pretval & (SMACQ_PRODUCE|SMACQ_CANPRODUCE)) {
			/* Come back for more */
			runable(runqp, f, NULL);
		}

		if (f->status & SMACQ_END) {
			if (! (pretval & (SMACQ_PASS|SMACQ_PRODUCE|SMACQ_CANPRODUCE))) {
				if (pending(*runqp, f)) {
					/* Defer shutdown */
					runable(runqp, f, NULL);
				} else {
					/* Produced nothing */
					pretval |= SMACQ_END;
					//fprintf(stderr, "%s produced nothing while ending\n", f->name);
				}
			}
		}

		if (pretval & SMACQ_END) {
	  		do_shutdown(runqp, f);

			assert(!(pretval & (SMACQ_PRODUCE|SMACQ_CANPRODUCE)));

			return 1;
		}
	}

	return 0;
}
      
static int run_consume(smacq_graph * f, const dts_object * d, struct runq ** runqp, smacq_graph * startf) {
        int outchan = -1;
	int result = 0;

	smacq_result retval = f->ops.consume(f->state, d, &outchan);

	if (retval & SMACQ_PASS) {
	  //fprintf(stderr, "Pass on %p by %p\n", d, f);
	  queue_children(runqp, f, d, outchan);
	}
	dts_decref(d);

        if (retval & (SMACQ_PRODUCE|SMACQ_CANPRODUCE)) {
	      if (run_produce(f, runqp, startf)) {
		      result = 1;
	      }
        }

        if (retval & SMACQ_END) {
 	  do_shutdown(runqp, f);
	  result = 1;
        }

	return result;
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
int smacq_sched_iterative(smacq_graph * startf, const dts_object * din, const dts_object ** dout , void ** state, int produce_first) {
  struct runq ** runqp = (struct runq **)state;

  if (din) {
    runable(runqp, startf, din);
  }

  *dout = NULL;

  if (!*runqp) {
     if (produce_first) {
	  while(startf) {
		  //fprintf(stderr, "produce_first for %p\n", startf);
        	  /* Force first guy to produce */
		  runable(runqp, startf, NULL);
		  startf = startf->next_graph;
	  }
     }
  }

  if (*runqp) {
      smacq_graph * f = (*runqp)->f;
      const dts_object * d = (*runqp)->d;
      int shutdown;

      pop_runable(runqp);

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
	shutdown = run_consume(f, d, runqp, startf);
      } else {
	/* Force produce */
	shutdown = run_produce(f, runqp, startf);
      }

      if (shutdown && (! (*runqp || smacq_sched_iterative_graph_alive(startf)))) 
  	return SMACQ_END;

  } else if (produce_first) {
    return SMACQ_END;
  }

  return SMACQ_FREE;
}


