#include <smacq.h>
#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include <smacq.h>

const dts_object* _flow_alloc(int size, int type) {
  dts_object * d = g_malloc(sizeof(dts_object) + size);

  d->len=size;
  d->type=type;
  d->data=d+1;  // Inline data
  d->numfields = 0;
  d->fields = NULL;
  d->refcount=1;

  pthread_mutex_init(&d->mutex, NULL);

  return d;
}




