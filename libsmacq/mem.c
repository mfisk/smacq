#include <smacq.h>
#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include <smacq.h>

#define MINSIZE 100

static inline void dts_init_object(dts_object * d) {
  d->free_data=0;
  d->refcount=1;
  d->data=d+1;  // Inline data
}

#ifndef max
#define max(a,b) ((a)>(b) ? (a) : (b))
#endif

static inline const dts_object* dts_alloc_slow(dts_environment * tenv, int size, int type) {
  dts_object * d;
  int max_size;

  max_size = max(size, MINSIZE);
  d = g_malloc0(sizeof(dts_object) + max_size);

#ifndef SMACQ_OPT_NOPTHREADS
  pthread_mutex_init(&d->mutex, NULL);
#endif

  d->len=size;
  d->max_size = max_size;
  d->type=type;
  d->tenv = tenv;

  darray_init(&d->fields, 0);
  dts_init_object(d);
  return d;
}

const dts_object* dts_alloc(dts_environment * tenv, int size, int type) {
  const dts_object * o = *tenv->freelist.p;

  if (o && (size <= o->max_size)) {
    dts_init_object((dts_object*)o);

    *tenv->freelist.end = NULL;

    if (tenv->freelist.p > tenv->freelist.start) {
      tenv->freelist.p--;
    }
  } else {
    o = dts_alloc_slow(tenv, size, type);
  }
  return o;
}

void dts_free(const dts_object * d) {
  if (d->tenv->freelist.p < d->tenv->freelist.end) {
  	d->tenv->freelist.p++;
  	*d->tenv->freelist.p = d;
  } else {
	free((void*)d);
  }
}
