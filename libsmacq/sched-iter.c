#include <smacq-internal.h>
#include <stdlib.h>

struct runq {
  struct filter * f;
  const dts_object * d;
  struct runq * next;
};

void runable(struct runq ** runqp, struct filter * f, const dts_object * d) {
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

void queue_children(struct runq ** runq, struct filter * f, const dts_object * d, int outchan) {
  // enqueue with NULL f if there are no children.

  if (outchan >= 0) {
    assert(outchan < f->numchildren);
    runable(runq, f->next[outchan], d);
  } else {
    int i;
    int found = 0;

    for (i=0; i < f->numchildren; i++) {
      if (f->next[i]) {
	runable(runq, f->next[i], d);
	found = 1;
      }
    }
    
    if (!found) runable(runq, NULL, d);
  }
}


void do_shutdown(struct runq ** runqp, struct filter *);

void check_for_shutdown(struct runq ** runqp, struct filter *f) {
  int i;
  for (i=0; i < f->numchildren; i++) {
    if (f->next[i] && (! (f->next[i]->status & SMACQ_END))) 
      return;
  }

  /* No more active children.  Clean-up self and parents. */
  for (i=0; i < f->numchildren; i++) {
    if (f->next[i] && (! (f->next[i]->status & SMACQ_END))) 
      flow_sched_iterative_shutdown(f->next[i], (void**)runqp);
  }
  //do_shutdown(runqp, f);
}

void do_shutdown(struct runq ** runqp, struct filter *f) {
  int i;

  if (f->shutdown) {
    f->shutdown(f->state);
  }

  //fprintf(stderr, "module %p %s ended\n", f, f->name);
  f->status = SMACQ_END;

  /* Propagate to children */
  for (i=0; i < f->numchildren; i++) {
    /* Enqueue a terminate record */
    if (! (f->next[i]->status & SMACQ_END)) {
      flow_sched_iterative_shutdown(f->next[i], (void**)runqp);
    }
  }

  /* Propagate to parents */
  for (i=0; i< f->numparents; i++) {
    check_for_shutdown(runqp, f->parent[i]);
  }
}

static int flow_sched_iterative_graph_alive (struct filter *f) {
  int i;

  if (! (f->status & SMACQ_END)) {
    return 1;
  }

  for (i=0; i<f->numchildren; i++) {
    if (f->next[i]) {
      if (flow_sched_iterative_graph_alive(f->next[i])) {
	return 1;
      }
    }
  }

  return 0;
}

void flow_sched_iterative_shutdown(struct filter * startf, void ** state) {
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
int flow_sched_iterative(struct filter * startf, const dts_object * din, const dts_object ** dout , void ** state, int produce_first) {
  int retval;
  struct runq ** runqp = (struct runq **)state;

  if (din)
    runable(runqp, startf, din);
  
  while (1) {
    if (!(*runqp)  && (produce_first)) {

      /* Force first guy to produce */
      int outchan = -1;
      const dts_object * d;
      
      retval = startf->produce(startf->state, &d, &outchan);
      //fprintf(stderr, "Forced a produce by %p, got %p for %d\n", startf, d, outchan);
      
      queue_children(runqp, startf, d, outchan);
      
      if (retval & SMACQ_END) {
	/* Enqueue a terminate record */
	flow_sched_iterative_shutdown(startf, (void**)runqp);
      }
    }
    
    if (!(*runqp)) {
      *dout = NULL;
      return SMACQ_FREE;
    } else {
      struct filter * f = (*runqp)->f;
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
	  //(*runqp)->f = NULL;
	  *dout = NULL;
	  if (*runqp || flow_sched_iterative_graph_alive(startf)) {
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

	retval = f->consume(f->state, d, &outchan);

	if (retval & SMACQ_PASS) {
	  //fprintf(stderr, "Pass on %p by %p\n", d, f);
	  queue_children(runqp, f, d, outchan);
	}

	dts_decref(d);
      }

      if (retval & (SMACQ_PRODUCE|SMACQ_CANPRODUCE)) {
	int outchan = -1;
	const dts_object * d = NULL;
	pretval = f->produce(f->state, &d, &outchan);
	
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


