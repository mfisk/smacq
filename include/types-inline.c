#ifndef SMACQ_INLINE_C
#define SMACQ_INLINE_C

#include <smacq.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

void msg_send(dts_environment *, dts_field_element, dts_object *, dts_comparison *);
dts_object * dts_construct(dts_environment * tenv, int type, void * data);

static inline void * memdup(void * buf, int size) {
  void * copy = malloc(size);
  memcpy(copy, buf, size);
  return copy;
}

static inline void * crealloc(void * ptr, int newsize, int oldsize) {
  void * newp = realloc(ptr, newsize);
  //fprintf(stderr, "crealloc of %p from %d to %d\n", ptr, oldsize, newsize);
  if (!newp) return NULL;
  memset((void *)((int)newp+oldsize), 0, newsize-oldsize);
  return newp;
}

static inline void * darray_get(struct darray * darray, int element) {
  if (element > darray->max) {
	return NULL;
  }
  return (void*)darray->array[element];
}

static void darray_set(struct darray * darray, unsigned int element, void * value) {
  if (element > darray->max) {
	darray->array = crealloc(darray->array, 
				 (element+1) * sizeof(unsigned long), 
				 (darray->max+1) * sizeof(unsigned long));
	darray->max = element;
  }
  darray->array[element] = (unsigned long)value;
}

static inline void darray_free(struct darray * darray) {
  free(darray->array);
  darray->array = NULL;
  darray->max = -1;
}

static inline void darray_init(struct darray * darray, int max) {
  assert(sizeof(unsigned long) == sizeof(void *));
  darray->array = calloc(max+1, sizeof(unsigned long));
  darray->max = max;
}

static inline void dts_incref(const dts_object * d_const, int i) {
  dts_object * d = (dts_object*)d_const;
#ifndef SMACQ_OPT_NOPTHREADS
  pthread_mutex_lock(&d->mutex);
#endif
  d->refcount += i;
#ifndef SMACQ_OPT_NOPTHREADS
  pthread_mutex_unlock(&d->mutex);
#endif
}

static void dts_decref(const dts_object * d_const) {
  dts_object * d = (dts_object*)d_const;
  int i;

  //fprintf(stdout, "deref called for %p\n", d);

  assert(d->refcount > 0);

#ifndef SMACQ_OPT_NOPTHREADS
  pthread_mutex_lock(&d->mutex);
#endif
  d->refcount--;
  if (!d->refcount) { 
#ifndef SMACQ_OPT_NOPTHREADS
    pthread_mutex_unlock(&d->mutex);
#endif

    // Deref children too then
    for (i=0; i<=d->fields.max; i++) {
      if (d->fields.array[i]) {
	//fprintf(stderr, "child: %p... ", d->fields.array[i]);
	dts_decref(darray_get(&d->fields, i));
      }
    }

    //fprintf(stderr, "freeing %p\n", d);

    darray_free(&d->fields);
    //XXX: if (d->free_data) free(d->data);
    free(d);
#ifndef SMACQ_OPT_NOPTHREADS
  } else {
    pthread_mutex_unlock(&d->mutex);
#endif
  }
}

static inline dts_object * smacq_dts_construct(smacq_environment * env, int type, void * data) {
  return dts_construct(env->types, type, data);
}

#define dts_field_first(x) (x[0])

static inline void dts_field_free(dts_field field) {
  return free(field);
}

static inline dts_field dts_field_next(dts_field field) {
  if (field[0])
  	return field+1;
  else
	return NULL;
}

static inline void dts_attach_field_single(const dts_object * d, dts_field_element field, const dts_object * field_data) {
  darray_set( (struct darray *)&d->fields, field, (dts_object*)field_data);
}

static inline void dts_attach_field(const dts_object * d, dts_field field, const dts_object * field_data) {
  int fnum = dts_field_first(field);

  dts_attach_field_single(d, fnum, field_data);

  assert(!dts_field_first(dts_field_next(field)));
}

/* Syscalls */

/* Test whether datum has specified type 
static inline int smacq_asktype(const dts_object * d, int type) {
  return(d->type == type);
}
*/

static inline int dts_getsize(const dts_object * d) {
 return(d->len);
}

static inline void * dts_getdata(const dts_object * d) {
 return(d->data);
}

static inline int dts_gettype(const dts_object * d) {
 return(d->type);
}

static inline void smacq_msg_send(smacq_environment * env, dts_field field, dts_object * d, dts_comparison * comps) {
  assert(!dts_field_first(dts_field_next(field)));
  return(msg_send(env->types, dts_field_first(field), d, comps));
}

/* Make datum have specified type */
static inline int smacq_datum_settype(const dts_object * d, int type) {
  /* Avoid const warning */
  dts_object * wd = (dts_object *)d;
  return((wd->type = type));
}

/* Allocate space for a new datum */
static inline const dts_object * smacq_alloc(smacq_environment * env, int size, int type) {
  return(env->alloc(size, type));
}

static inline const dts_object * dts_writable(smacq_environment * env, const dts_object *d) {
  if (d->refcount > 1) {
    const dts_object * newp = smacq_alloc(env, d->len, d->type);
    memcpy(newp->data, d->data, d->len);
    dts_incref(newp, 1);

    return newp;
  } else {
    // We're the only user, so just modify this one
    dts_incref(d, 1);
    return d;
  }
}

static inline int smacq_requiretype(smacq_environment * env, char * tname) {
  return(env->types->requiretype(env->types, tname));
}

static inline dts_field smacq_requirefield(smacq_environment * env, char * tname) {
  return(env->types->requirefield(env->types, tname));
}

static inline dts_field dts_requirefield(dts_environment * env, char * tname) {
  return(env->requirefield(env, tname));
}

/* Parse a string into a type's native storage */
static inline int smacq_fromstring(smacq_environment * env, int type, char * datastr, dts_object * data) {
  return(env->types->fromstring(env->types, type, datastr, data));
}

static inline int dts_lt(dts_environment * env, int type, void * v1, int l1, void *v2, int l2) {
  return(env->lt(env, type, v1, l1, v2, l2));
}

/* Get the named field from a datum */
static inline int smacq_getfield(smacq_environment * env, const dts_object * datum, dts_field field, dts_object * data) {
  return(env->types->getfield(env->types, datum, field, data));
}

/* Get the named field from a datum */
static inline int dts_getfield(dts_environment * env, const dts_object * datum, dts_field field, dts_object * data) {
  return(env->getfield(env, datum, field, data));
}

/* Same as above, but return a new copy of the data */
static inline int smacq_getfield_copy(smacq_environment * env, const dts_object * datum, dts_field field, dts_object * field_data) {
	int retval = smacq_getfield(env, datum, field, field_data);
	if (retval) 
		field_data->data = memdup(field_data->data, field_data->len);
	return retval;
}

#define dts_data_as(datum,type) (*((type*)((datum)->data)))
	
/* Convert a type number to a type name */
static inline char * dts_typename_bynum(smacq_environment * env, int num) {
  return(env->types->typename_bynum(env->types, num));
}

/* Convert a type name to a type number */
static inline int smacq_opt_typenum_byname(smacq_environment * env, char * name) {
  return(env->types->typenum_byname(env->types, name));
}


/*
 * Interface to data testing system
 */
int type_parsetest(dts_environment * tenv, dts_comparison * comp, char * test);
int type_match(dts_environment * tenv, const dts_object * datum, 
	       dts_comparison * comps, int same_types);

static inline int smacq_parsetest(smacq_environment * env, 
				 dts_comparison * comp, char * test) {
  return(type_parsetest(env->types, comp, test));
}

static inline int smacq_match(smacq_environment * env, 
			     const dts_object * datum, 
			     dts_comparison * comps, int same_types) {
  return(type_match(env->types, datum, comps, same_types));
}

#endif

