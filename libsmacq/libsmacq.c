#include <smacq.h>
#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include <smacq.h>

void dts_init_object(dts_object * d) {
  d->free_data=0;
  d->refcount=1;
#ifndef SMACQ_OPT_NOPTHREADS
  pthread_mutex_init(&d->mutex, NULL);
#endif

  darray_init(&d->fields, 0);
}

const dts_object* _smacq_alloc(int size, int type) {
  dts_object * d = g_malloc(sizeof(dts_object) + size);
  d->data=d+1;  // Inline data

  d->len=size;
  d->type=type;

  dts_init_object(d);
  return d;
}




