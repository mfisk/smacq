#ifndef SMACQ_INLINE_C
#define SMACQ_INLINE_C

#include <smacq.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

void msg_send(dts_environment *, int, dts_object *, dts_comparison *);
dts_object * dts_construct(dts_environment * tenv, int type, void * data);

static inline void * memdup(void * buf, int size) {
  void * copy = malloc(size);
  memcpy(copy, buf, size);
  return copy;
}

static inline void dts_incref(const dts_object * d_const, int i) {
  dts_object * d = (dts_object*)d_const;
  pthread_mutex_lock(&d->mutex);
  d->refcount += i;
  pthread_mutex_unlock(&d->mutex);
}

static void dts_decref(const dts_object * d_const) {
  dts_object * d = (dts_object*)d_const;
  int i;

  //fprintf(stdout, "deref called for %p\n", d);

  assert(d->refcount > 0);

  pthread_mutex_lock(&d->mutex);
  d->refcount--;
  if (!d->refcount) { 
    pthread_mutex_unlock(&d->mutex);

    // Deref children too then
    for (i=0; i<d->numfields; i++) {
      if (d->fields[i]) {
	//fprintf(stderr, "child: %p... ", d->fields[i]);
	dts_decref(d->fields[i]);
      }
    }

    //fprintf(stderr, "freeing %p\n", d);

    free(d->fields);
    free(d);
  } else {
    pthread_mutex_unlock(&d->mutex);
  }
}

static inline dts_object * flow_dts_construct(smacq_environment * env, int type, void * data) {
  return dts_construct(env->types, type, data);
}

static inline void * crealloc(void * ptr, int newsize, int oldsize) {
  void * newp = realloc(ptr, newsize);
  if (!newp) return NULL;
  memset((void *)((int)newp+oldsize), 0, newsize-oldsize);
  return newp;
}

static inline void dts_attach_field(const dts_object * d, int field, const dts_object * field_data) {
  dts_object * wd = (dts_object *)d;
  if (field >= d->numfields) {
    wd->fields = (const dts_object **)crealloc(d->fields, (field+1)*sizeof(void*), d->numfields*sizeof(void*));
    assert(wd->fields);
    wd->numfields = field+1;
  }

  //dts_incref(d, 1); Probably should
  d->fields[field] = field_data;
}

/* Syscalls */

/* Test whether datum has specified type 
static inline int flow_asktype(const dts_object * d, int type) {
  return(d->type == type);
}
*/

static inline int flow_datum_size(const dts_object * d) {
 return(d->len);
}

static inline void * dts_getdata(const dts_object * d) {
 return(d->data);
}

static inline int dts_gettype(const dts_object * d) {
 return(d->type);
}

static inline void flow_msg_send(smacq_environment * env, int field, dts_object * d, dts_comparison * comps) {
  return(msg_send(env->types, field, d, comps));
}

/* Make datum have specified type */
static inline int flow_datum_settype(const dts_object * d, int type) {
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

static inline int flow_requiretype(smacq_environment * env, char * tname) {
  return(env->types->requiretype(env->types, tname));
}

static inline int flow_requirefield(smacq_environment * env, char * tname) {
  return(env->types->requirefield(env->types, tname));
}

static inline int dts_requirefield(dts_environment * env, char * tname) {
  return(env->requirefield(env, tname));
}

/* Parse a string into a type's native storage */
static inline int flow_fromstring(smacq_environment * env, int type, char * datastr, dts_object * data) {
  return(env->types->fromstring(env->types, type, datastr, data));
}

static inline int dts_lt(dts_environment * env, int type, void * v1, int l1, void *v2, int l2) {
  return(env->lt(env, type, v1, l1, v2, l2));
}

/* Transform typed data to a representation (such as "string") */
static inline int flow_presentdata(smacq_environment * env, dts_object * data, int transform, void ** resultstr, int * reslen) {
  return(env->types->presentdata(env->types, data, transform, resultstr, reslen));
}

int dts_transform(dts_environment * env, char * name);

static inline int flow_transform(smacq_environment * env, char * name) {
  return(dts_transform(env->types, name));
}

static inline int dts_presentdata(dts_environment * env, dts_object * data, int transform, void ** resultstr, int * reslen) {
  return(env->presentdata(env, data, transform, resultstr, reslen));
}

/* Get the named field from a datum */
static inline int flow_getfield(smacq_environment * env, const dts_object * datum, int field, dts_object * data) {
  return(env->types->getfield(env->types, datum, field, data));
}

/* Get the named field from a datum */
static inline int dts_getfield(dts_environment * env, const dts_object * datum, int field, dts_object * data) {
  return(env->getfield(env, datum, field, data));
}

/* Same as above, but return a new copy of the data */
static inline int flow_getfield_copy(smacq_environment * env, const dts_object * datum, int field, dts_object * field_data) {
	int retval = flow_getfield(env, datum, field, field_data);
	if (retval) 
		field_data->data = memdup(field_data->data, field_data->len);
	return retval;
}
	
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

static inline int flow_parsetest(smacq_environment * env, 
				 dts_comparison * comp, char * test) {
  return(type_parsetest(env->types, comp, test));
}

static inline int flow_match(smacq_environment * env, 
			     const dts_object * datum, 
			     dts_comparison * comps, int same_types) {
  return(type_match(env->types, datum, comps, same_types));
}

#endif

