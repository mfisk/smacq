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
};

void runable(struct runq ** runqp, smacq_graph * f, const dts_object * d) {
  struct runq * entry;

  if (f && (f->status & SMACQ_END)) return;

  //fprintf(stderr, "runable %p type %d\n", d, dts_gettype(d));
  assert(!d || dts_gettype(d));

  entry = malloc(sizeof(struct runq));
  entry->f = f;
  entry->d = d;
  entry->next = *runqp;
  *runqp = entry;
  if (d) dts_incref(d, 1);
}
  
static void pop_runable(struct runq ** runqp) {
  struct runq * runq = *runqp;
  assert(runq);
  *runqp=runq->next;
  free(runq);
}

void queue_children(struct runq ** runq, smacq_graph * f, const dts_object * d, int outchan) {
  // enqueue with NULL f if there are no children.

  if (outchan >= 0) {
    assert(outchan < f->numchildren);
    runable(runq, f->child[outchan], d);
  } else {
    int i;
    int found = 0;

    for (i=0; i < f->numchildren; i++) {
      if (f->child[i]) {
	runable(runq, f->child[i], d);
	found = 1;
      }
    }
    
    if (!found) runable(runq, NULL, d);
  }
}


void do_shutdown(struct runq ** runqp, smacq_graph *);

void check_for_shutdown(struct runq ** runqp, smacq_graph *f) {
  int i;
  for (i=0; i < f->numchildren; i++) {
    if (f->child[i] && (! (f->child[i]->status & SMACQ_END))) 
      return;
  }

  /* No more active children.  Clean-up self and parents. */
  for (i=0; i < f->numchildren; i++) {
    if (f->child[i] && (! (f->child[i]->status & SMACQ_END))) 
      smacq_sched_iterative_shutdown(f->child[i], (void**)runqp);
  }
  //do_shutdown(runqp, f);
}

void do_shutdown(struct runq ** runqp, smacq_graph *f) {
  int i;

  if (f->status & SMACQ_FREE) {
	  /* Already shutdown, so do nothing */
	  return;
  }

  if (f->ops.shutdown) {
    f->ops.shutdown(f->state);
  }
  f->state = NULL;  /* Just in case */

  // fprintf(stderr, "module %p %s ended\n", f, f->name);
  f->status = SMACQ_END|SMACQ_FREE;

  /* Propagate to children */
  for (i=0; i < f->numchildren; i++) {
    /* Enqueue a terminate record */
    if (! (f->child[i]->status & SMACQ_END)) {
      smacq_sched_iterative_shutdown(f->child[i], (void**)runqp);
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

void smacq_sched_iterative_shutdown(smacq_graph * startf, void ** state) {
  struct runq ** runqp = (struct runq **)state;

  struct runq * entry = malloc(sizeof(struct runq));
  entry->f = startf;
  entry->d = NULL;
  entry->next = NULL;

  // Append at end of queue
  if (! *runqp) {
    *runqp = entry;
  } else {
    struct runq * next;

    for (next=*runqp; next->next; next=next->next);
    next->next = entry;
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
int smacq_sched_iterative(smacq_graph * startf, const dts_object * din, const dts_object ** dout , void ** state, int produce_first) {
  int retval;
  struct runq ** runqp = (struct runq **)state;

  if (din)
    runable(runqp, startf, din);
  
  while (1) {
    if (!(*runqp)) {
      if (produce_first) {

        /* Force first guy to produce */
        int outchan = -1;
        const dts_object * d = NULL;
      
        retval = startf->ops.produce(startf->state, &d, &outchan);
        //fprintf(stderr, "Forced a produce by %p, got %p for %d\n", startf, d, outchan);
       
        if (retval & SMACQ_PASS) {
      	  queue_children(runqp, startf, d, outchan);
      	  dts_decref(d);
        }
      
        if (retval & SMACQ_END) {
	  /* Enqueue a terminate record */
	  smacq_sched_iterative_shutdown(startf, (void**)runqp);
        }
      } else {
      	*dout = NULL;
      	return SMACQ_FREE;
      }
    } else {
      smacq_graph * f = (*runqp)->f;
      const dts_object * d = (*runqp)->d;
      int outchan = -1;
      int pretval = 0;

      if (!f) {
	/* Datum fell off end of data-flow graph */
	pop_runable(runqp);

	if (dout) {
	  *dout = d;
	  return SMACQ_PASS;
	} else {
	  dts_decref(d);
	  continue;
	}
      }
      
      if (!d) {
	if (f->status & SMACQ_END) {
	  pop_runable(runqp);

	  do_shutdown(runqp, f);

	  *dout = NULL;
	  if (*runqp || smacq_sched_iterative_graph_alive(startf)) {
	    return SMACQ_FREE;
	  } else {
	    assert(!*runqp);
	    return SMACQ_END;
	  } 
	} else {
	  /* Last call */
	  /* We don't pop_runable so that we'll come back again and do_shutdown after last call */
	  f->status = SMACQ_END;
	  retval = SMACQ_PRODUCE;
	}
      } else {
	pop_runable(runqp);

	retval = f->ops.consume(f->state, d, &outchan);

	if (retval & SMACQ_PASS) {
	  //fprintf(stderr, "Pass on %p by %p\n", d, f);
	  queue_children(runqp, f, d, outchan);
	}

	dts_decref(d);
      }

      if (retval & (SMACQ_PRODUCE|SMACQ_CANPRODUCE)) {
	int outchan = -1;
	const dts_object * d = NULL;
	pretval = f->ops.produce(f->state, &d, &outchan);

	if (pretval & SMACQ_PASS)
		queue_children(runqp, f, d, outchan);
      }
      
      if ((retval|pretval) & SMACQ_END) {
	do_shutdown(runqp, f);
      }

    }
  }

  *dout = NULL;
  return SMACQ_FREE;
}


