#include <smacq.h>
#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include <smacq.h>

#define MINSIZE 100

#ifdef SMACQ_DEBUG_MEM
#define SDEBUG(x) x
#else
#define SDEBUG(x)
#endif

static unsigned long Id = 0;

static inline void dts_init_object(dts_object * d) {
  d->free_data=0;
  d->refcount=1;
  d->data=d+1;  // Inline data
  d->id=Id++;
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
  const dts_object * o;
  
  SDEBUG(dts_object_virtual_count++);

  if (tenv->freelist.p >= tenv->freelist.start) {
    o = *tenv->freelist.p;
    tenv->freelist.p--;
    //fprintf(stderr, "dts_alloc reusing %p\n", o);

    if (size > o->max_size) {
	    o = realloc((void*)o, size + sizeof(dts_object));
	    ((dts_object*)o)->max_size = size;
    }

    dts_init_object((dts_object*)o);
    ((dts_object*)o)->type = type;
    ((dts_object*)o)->len = size;

  } else {
    o = dts_alloc_slow(tenv, size, type);
    //fprintf(stderr, "dts_alloc creating %p\n", o);
  }
  return o;
}

void dts_free(const dts_object * d) {
  if (d->tenv->freelist.p < d->tenv->freelist.end) {
    	//fprintf(stderr, "dts_free saving %p at %p between %p and %p\n", d, d->tenv->freelist.p+1, d->tenv->freelist.start, d->tenv->freelist.end);
  	d->tenv->freelist.p++;
  	*d->tenv->freelist.p = d;
  } else {
	//fprintf(stderr,"dts_free freeing %p\n", d);
  	darray_free((struct darray *)(&d->fields));
	free((void*)d);
	SDEBUG(dts_object_count--);
  }
}
