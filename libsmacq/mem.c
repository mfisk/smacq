#include <smacq.h>
#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include <smacq.h>

#define MINSIZE 100

#ifdef SMACQ_DEBUG
#define SDEBUG(x) x
#else
#define SDEBUG(x)
#endif

static inline void dts_init_object(dts_object * d) {
  d->free_data=0;
  d->refcount=1;
  d->data=d+1;  // Inline data
}

#ifndef max
#define max(a,b) ((a)>(b) ? (a) : (b))
#endif

SDEBUG(static int dts_object_count = 0);
SDEBUG(static int dts_object_virtual_count = 0);

static inline const dts_object* dts_alloc_slow(dts_environment * tenv, int size, int type) {
  dts_object * d;
  int max_size;

  max_size = max(size, MINSIZE);
  d = g_malloc0(sizeof(dts_object) + max_size);
  SDEBUG(dts_object_count++);
  SDEBUG(fprintf(stderr, "%d DTS objects in existence, %d virtual objects used\n", dts_object_count, dts_object_virtual_count));

#ifndef SMACQ_OPT_NOPTHREADS
  pthread_mutex_init(&d->mutex, NULL);
#endif

  d->len=size;
  d->max_size = max_size;
  d->type=type;
  d->tenv = tenv;

  darray_init(&d->fields, tenv->max_field);
  dts_init_object(d);
  return d;
}

const dts_object* dts_alloc(dts_environment * tenv, int size, int type) {
  const dts_object * o = *tenv->freelist.p;

  SDEBUG(dts_object_virtual_count++);

  if (o) {
    if (size > o->max_size) {
	    o = realloc((void*)o, size + sizeof(dts_object));
	    ((dts_object*)o)->max_size = size;
    }

    dts_init_object((dts_object*)o);
    ((dts_object*)o)->type = type;
    ((dts_object*)o)->len = size;

    //fprintf(stderr, "dts_alloc reusing %p\n", o);

    if (tenv->freelist.p > tenv->freelist.start) {
      tenv->freelist.p--;
    } else {
      *tenv->freelist.p = NULL;
    }
  } else {
    o = dts_alloc_slow(tenv, size, type);
  }
  return o;
}

void dts_free(const dts_object * d) {
  if (d->tenv->freelist.p < d->tenv->freelist.end) {
	if (*d->tenv->freelist.p != NULL)
  		d->tenv->freelist.p++;
  	*d->tenv->freelist.p = d;
  } else {
  	darray_free((struct darray *)(&d->fields));
	free((void*)d);
	//fprintf(stderr,"freeing empty o %p\n", d);
	SDEBUG(dts_object_count--);
  }
}
