#ifndef BUFFER_INLINE_C
#define BUFFER_INLINE_C
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>
#include <flow-internal.h>

static inline void ring_enqueue(struct filter * f, dts_object * d) {
  pthread_mutex_lock(&f->qlock);

  while (f->q[f->ring_produce]) 
    pthread_cond_wait(&f->ring_notfull, &f->qlock);

  f->q[f->ring_produce] = d;
  f->ring_produce = ++f->ring_produce % f->ringsize;
  pthread_cond_broadcast(&f->ring_notempty);

  pthread_mutex_unlock(&f->qlock);
}

static inline dts_object * ring_dequeue(struct filter * f) {
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

static void flow_passalong(struct filter * f, dts_object * d, int outchan) {
  int i;

  if (!f->next) return;

  if (d != RING_EOF) // Pass -1 as EOF, but don't try to reference it
    dts_incref(d, f->numchildren);

  if (outchan >= 0) {
	assert(outchan < f->numchildren);
	ring_enqueue(f->next[outchan], d);
  } else {
  	for (i=0; i < f->numchildren; i++) {
    		//if (f->next[i] != recurse_except) 
      		ring_enqueue(f->next[i], d);
	}
  }
}


static inline void flow_passall(struct filter * f, dts_object * d) {
  struct filter * top = f;
  while(top->previous) top = top->previous;
  
  flow_passalong(top, d, -1);
}

static void flow_canceldownto(struct filter * f, struct filter * until) {
  int i;

  if (f == until) return;

  for (i=0; i < f->numchildren; i++) {
      flow_canceldownto(f->next[i], until);
  }

  pthread_cancel(f->thread);
}

static inline void flow_cancelupstream(struct filter * f) {
  struct filter * top = f;

  while(top->previous) top = top->previous;

  flow_canceldownto(top, f);
}

#endif

