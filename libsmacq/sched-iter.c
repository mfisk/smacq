#include <flow-internal.h>
#include <stdlib.h>

struct runq {
  struct filter * f;
  const dts_object * d;
  struct runq * next;
};

void runable(struct runq ** runqp, struct filter * f, const dts_object * d) {
  struct runq * entry = malloc(sizeof(struct runq));
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
    if (f->next[i]) return;
  }

  /* No more active children */
  do_shutdown(runqp, f);
}

void do_shutdown(struct runq ** runqp, struct filter *f) {
  int i, j;

  if (f->shutdown) {
    f->shutdown(f->state);
  }

  for (i=0; i< f->numparents; i++) {
    for (j=0; j < f->parent[i]->numchildren; j++) {
      if (f->parent[i]->next[j] == f) {
	f->parent[i]-> next[j] = NULL;
	check_for_shutdown(runqp, f->parent[i]);
      }
    }
  }

  smacq_free_module(f);
}
    
void flow_sched_iterative_shutdown(struct filter * startf, void ** state) {
  struct runq ** runqp = (struct runq **)state;
  runable(runqp, startf, NULL);
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
	/* Enqueue a terminate recorde */
	runable(runqp, startf, NULL);
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

      if (!d) {
	/* Must terminate */
	/* They shouldn't try again, but just in case, we'll leave the terminate record on the queue,
	   but won't shutdown the graph again on subsequent calls */
	if (f) do_shutdown(runqp, f);
	(*runqp)->f = NULL;
	*dout = NULL;
	return SMACQ_END;
      }

      //fprintf(stderr, "Pulled %p off queue for module %p\n", d, f);
      
      pop_runable(runqp);
      
      if (!f) {
	/* Datum fell off end of data-flow graph */
	if (dout) {
	  *dout = d;
	  return SMACQ_PASS;
	} else {
	  dts_decref(d);
	  continue;
	}
      }
      
      retval = f->consume(f->state, d, &outchan);
      
      if (retval & (SMACQ_PRODUCE|SMACQ_CANPRODUCE)) {
	int outchan = -1;
	const dts_object * d;
	pretval = f->produce(f->state, &d, &outchan);
	
	queue_children(runqp, f, d, outchan);
      }
      
      if (retval & SMACQ_PASS) {
	//fprintf(stderr, "Pass on %p by %p\n", d, f);
	queue_children(runqp, f, d, outchan);
      }
      
      if ((retval|pretval) & SMACQ_END) {
	do_shutdown(runqp, f);
      }
      
      dts_decref(d);
    }
  }

  *dout = NULL;
  return SMACQ_FREE;
}


