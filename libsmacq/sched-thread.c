#include <smacq-internal.h>
#include <stdio.h>


struct thread_args {
  smacq_graph * f;
  struct smacq_init * context;
};

// Return 0 iff module has nothing more to produce at the moment
static inline int sched_force_produce(smacq_graph * f) {
  const dts_object * d = NULL;
  int output = -1;
  int retval = f->ops.produce(f->state, &d, &output);

  // Save producion status for later
  if (retval & SMACQ_PRODUCE) f->status |= SMACQ_PRODUCE;
  else f->status &= (~SMACQ_PRODUCE);

  if (retval & SMACQ_CANPRODUCE) f->status |= SMACQ_CANPRODUCE;
  else f->status &= (~SMACQ_CANPRODUCE);

  smacq_passalong(f, d == NULL ? RING_EOF : d, output);

  return (f->status & (SMACQ_CANPRODUCE|SMACQ_PRODUCE));
  //return !(retval & SMACQ_PASS);
}

/*
 * Produce if we can or have to, but avoid waiting if possible.
 * Return 0 iff nothing more to produce at the moment
 * Note: f->mutex must be held before entry
 */
static inline int sched_produce(smacq_graph * f) {
  if (f->status & SMACQ_PRODUCE) {
    return sched_force_produce(f);
  }
  
  if (f->status & SMACQ_CANPRODUCE) {
    // This produce is optional --- make sure we have room before proceeding

    // XXX:  there is race condition if there is fan-in to this child and the ring fills-up while we produce
    // XXX: should probably have a semaphore based reservation system on the child queues
    int i;
    for (i=0; i < f->numchildren; i++)
	if (f->child[i]->q[f->child[i]->ring_produce]) return 1; // Full

    return sched_force_produce(f);
  }

  return 0; // Not willing
}

/*
 * This is the main schedule for the multi-threaded implementation.
 * It runs until one of the following:
 *                  1. Our module returns a SMACQ_END
 *                  2. We receive a RING_EOF datum from upstream
 *                  3. Our module's produce function returned 0 (unexpected)
 */
void thread_sched(smacq_graph * f) {
  dts_object * d;
  int outchan;

  while (1) {
    pthread_testcancel(); // Give up here if requested

    // Our primary goal is always to push data to downstream
    // In addition, the module may have required a produce before we consume again
    if (!f->previous) {
      if (! sched_force_produce(f)) return;
      continue;
    }

    // Now we have nothing better to do than get data and consume it
    d = ring_dequeue(f);
    if (d == RING_EOF) {
      // fprintf(stderr, "Last call for %s\n", f->name);
      /* See if module wants to produce some final, unsolicited data  */
      f->status = 0;

      while (sched_force_produce(f)) {}
      
      smacq_passalong(f,d,-1);
      return;
    }
   
    outchan = -1;
    f->status = f->ops.consume(f->state, d, &outchan);

    // If the module wants to insert data, do it first
    while (sched_produce(f)) { 
	    // fprintf(stderr, "%s produced\n", f->name); 
    }
 
    if (f->status & SMACQ_PASS) 
      smacq_passalong(f, d, outchan);

    dts_decref(d);

    if (f->status & SMACQ_END) {
      smacq_passalong(f, RING_EOF, -1);
      smacq_cancelupstream(f);
      return;
    }

    //if (!r) return;
  }
} 


/*
 * Main routine for each child thread.
 * Then shutdown module and return (exit thread).
 */
void * thread_init(void * a) {
  smacq_graph * f = ((struct thread_args*)a)->f;

  //task = g_new0(struct task, 1);
  //task->module = f;

  thread_sched(f);

  if (f->ops.shutdown)
    f->ops.shutdown(f->state); 

  return NULL;
}  

/*
 * Recursively traverse the module tree and spawn a thread for each module.
 * Thread will run thread_init to initialize the module.
 */
void smacq_start_threads(smacq_graph * f) {
  int i;
  struct thread_args * a = g_new0(struct thread_args, 1);
  a->f = f;
  pthread_create(&f->thread, NULL, thread_init, a);

  for (i = 0; i < f->numchildren; i++ ) 
    smacq_start_threads(f->child[i]);
}
