#ifndef BUFFER_INLINE_C
#define BUFFER_INLINE_C
#ifndef SMACQ_OPT_NOPTHREADS
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>
#include <smacq.h>

static inline void ring_enqueue(smacq_graph * f, dts_object * d) {
  pthread_mutex_lock(&f->qlock);

  while (f->q[f->ring_produce]) 
    pthread_cond_wait(&f->ring_notfull, &f->qlock);

  f->q[f->ring_produce] = d;
  f->ring_produce = ++f->ring_produce % f->ringsize;
  pthread_cond_broadcast(&f->ring_notempty);

  pthread_mutex_unlock(&f->qlock);
}

static inline dts_object * ring_dequeue(smacq_graph * f) {
  dts_object * d;

  pthread_mutex_lock(&f->qlock);

  while (!f->q[f->ring_consume]) 
    pthread_cond_wait(&f->ring_notempty, &f->qlock);

  d = f->q[f->ring_consume];
  f->q[f->ring_consume] = NULL;
  f->ring_consume = ++f->ring_consume % f->ringsize;
  pthread_cond_broadcast(&f->ring_notfull);

  pthread_mutex_unlock(&f->qlock);
  return d;
}

static void smacq_passalong(smacq_graph * f, dts_object * d, int outchan) {
  int i;

  if (!f->child) return;

  if (d != RING_EOF) // Pass -1 as EOF, but don't try to reference it
    dts_incref(d, f->numchildren);

  if (outchan >= 0) {
	assert(outchan < f->numchildren);
	ring_enqueue(f->child[outchan], d);
  } else {
  	for (i=0; i < f->numchildren; i++) {
    		//if (f->child[i] != recurse_except) 
      		ring_enqueue(f->child[i], d);
	}
  }
}


static inline void smacq_passall(smacq_graph * f, dts_object * d) {
  smacq_graph * top = f;
  while(top->previous) top = top->previous;
  
  smacq_passalong(top, d, -1);
}

static void smacq_canceldownto(smacq_graph * f, smacq_graph * until) {
  int i;

  if (f == until) return;

  for (i=0; i < f->numchildren; i++) {
      smacq_canceldownto(f->child[i], until);
  }

  pthread_cancel(f->thread);
}

static inline void smacq_cancelupstream(smacq_graph * f) {
  smacq_graph * top = f;

  while(top->previous) top = top->previous;

  smacq_canceldownto(top, f);
}

#endif
#endif
