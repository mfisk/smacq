#ifndef SMACQ_INLINE_C
#define SMACQ_INLINE_C

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

struct smacq_engine_field {
  int type;
  struct dts_field_descriptor desc;
  int offset;
};

struct dts_type {
  struct darray fields;
  char * name;
  int num;
  GModule * module;
  struct dts_field_descriptor * description;
  struct dts_type_info info;
};

void msg_send(dts_environment *, dts_field_element, dts_object *, dts_comparison *);
dts_object * dts_construct(dts_environment * tenv, int type, void * data);
static void dts_decref(const dts_object * d_const);

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

static inline void dts_fieldcache_flush(const dts_object * datum, dts_field_element fnum) {
  dts_object * cached = darray_get((struct darray*)&datum->fields, fnum);
  dts_decref(cached); //Can't inline this (recursive)
  darray_set((struct darray*)&datum->fields, fnum, NULL);
  /*
  fprintf(stderr, "flushing child %p from cache of %p: %p\n", cached, datum,
  			darray_get((struct darray*)&datum->fields, fnum)
		  );
  */
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

    //fprintf(stderr, "Final decref of %p\n", d);
    
    // Flush field cache too then
    for (i=0; i<=d->fields.max; i++) {
      if (darray_get(&d->fields, i)) {
	dts_fieldcache_flush(d, i);
	//dts_decref(darray_get(&d->fields, i));
	//darray_set(&d->fields, i, NULL);
      }
    }

    if (d->free_data) {
	//fprintf(stderr, "Freeing data %p of %p\n", d->data, d);
    	free(d->data);
	d->data = NULL;
    }
    dts_free(d);
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
  const dts_object * old = darray_get( (struct darray *)&d->fields, field);
  //fprintf(stderr, "Attaching %p to %p field %d\n", field_data, d, field);
  darray_set( (struct darray *)&d->fields, field, (dts_object*)field_data);
  if (old) dts_decref(old);
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

const dts_object* dts_alloc(dts_environment * tenv, int size, int type);

/* Allocate space for a new datum */
static inline const dts_object * smacq_alloc(smacq_environment * env, int size, int type) {
  return(dts_alloc(env->types, size, type));
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

#define dts_data_as(datum,type) (*((type*)((datum)->data)))
	
/* Convert a type number to a type name */
static inline char * dts_typename_bynum(smacq_environment * env, int num) {
  return(env->types->typename_bynum(env->types, num));
}

/* Convert a type name to a type number */
static inline int smacq_opt_typenum_byname(smacq_environment * env, char * name) {
  return(env->types->typenum_byname(env->types, name));
}

static inline char * dts_fieldname_append(const char * old, const char * new) {
	char * ret = malloc(strlen(old) + strlen(new) + 2);
	sprintf(ret, "%s.%s", old, new);
	return ret;
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

static inline char * strcatn(char * dest, int len, char * src) {
	int left = len - strlen(dest) - 1;
	return strncat(dest, src, left);
}

static inline struct dts_type * dts_type_bynum(dts_environment * tenv, int num) {
  return darray_get(&tenv->types, num);
}


static inline const dts_object * dts_getfield_single(dts_environment * tenv, const dts_object * datum, dts_field_element fnum, dts_object * scratch) {
  dts_object * cached;
  int offset;

  cached = darray_get((struct darray*)&datum->fields, fnum);
  if (cached) {
    dts_incref(cached, 1);
    return cached;
  } else {
    struct smacq_engine_field * d;
    struct dts_type * t = dts_type_bynum(tenv, dts_gettype(datum));
    assert(t);

    d = darray_get(&t->fields, fnum);

  if (d) {
   int size = dts_type_bynum(tenv, d->type)->info.size;
   dts_object * field;
   offset = d->offset;
#ifndef SMACQ_OPT_FORCEFIELDCACHE
   if (!scratch) {
#endif 

    if (!d->desc.getfunc) {  
      assert(offset >= 0);
      //fprintf(stderr, "getfield has offset %d\n", offset);
      field = (dts_object*)dts_alloc(tenv, 0, d->type);
      field->len = size;
      field->data = datum->data+offset;
    } else {
      //fprintf(stderr, "getfield has helper func\n");
      field = (dts_object*)dts_alloc(tenv, size, d->type);
      if (!d->desc.getfunc(datum, field)) {
	/* getfunc failed, release memory */
	dts_decref(field);
	field = NULL;
      }
    }
#ifdef SMACQ_OPT_FORCEFIELDCACHE
    if (field) {
      dts_incref(field, 1);
      darray_set(&((dts_object*)datum)->fields, fnum, field);
    }
#endif
    return field;
#ifndef SMACQ_OPT_FORCEFIELDCACHE
   } else {
    scratch->type = d->type;
    scratch->len = size;
    
    if (offset >= 0) {
      //fprintf(stderr, "%s offset %d (constant), len = %d\n", name, offset, *len);
      scratch->data = dts_getdata(datum) + offset;
      return scratch;
    } else {
      //fprintf(stderr, "offset %d; calling getfunc()%p for %s\n", offset, d->desc.getfunc, name);
      
      assert(d->desc.getfunc);
      if (!d->desc.getfunc(datum, scratch)) {
     	return NULL;
      } else {
      	return scratch;
      }
    }
   }
#endif
  } else {
#ifndef SMACQ_OPT_NOMSGS
    return msg_check(tenv, datum, fnum, data);
#else
    return NULL;
#endif
  }
  }
}

#define SMACQ_OPT_NOSCRATCHFIELD

static inline const dts_object* dts_getfield(dts_environment * tenv, const dts_object * datum, dts_field fieldv, dts_object * scratch) {
  const dts_object * parent;
  const dts_object * f;

  parent = dts_getfield_single(tenv, datum, dts_field_first(fieldv), scratch);
  //fprintf(stderr, "Get field %d in %p from %p, next is %d\n", dts_field_first(fieldv), parent, datum, fieldv[1]);
  fieldv = dts_field_next(fieldv); 

  if (!dts_field_first(fieldv)) return parent;

  /* Must go deeper */
  while (1) {
    if (!parent) return parent;

    f = dts_getfield_single(tenv, parent, dts_field_first(fieldv), scratch);
    //fprintf(stderr, "Got field %d in %p from %p, next is %d\n", dts_field_first(fieldv), f, parent, fieldv[1]);

    /* Get rid of intermediate field */
    dts_decref(parent);

    fieldv = dts_field_next(fieldv); 
    if (!dts_field_first(fieldv) || !f)
      return f;

    parent = f;

#ifndef SMACQ_OPT_NOSCRATCHFIELD
    if (parent == scratch) {
      /* If getfield used the scratch space, make sure we don't use it again */
      scratch = NULL;
    }
#endif
  }
}

/* Get the named field from a datum */
static inline const dts_object * smacq_getfield(smacq_environment * env, const dts_object * datum, dts_field field, dts_object * data) {
  return(dts_getfield(env->types, datum, field, data));
}

/* Same as above, but return a new copy of the data */
static inline const dts_object * smacq_getfield_copy(smacq_environment * env, const dts_object * datum, dts_field field, dts_object * field_data) {
	dts_object * retval = (dts_object*)smacq_getfield(env, datum, field, field_data);
	if (retval && !retval->free_data) {
		retval->data = memdup(retval->data, retval->len);
		retval->free_data = 1;
	}
	return retval;
}


static inline const dts_object * smacq_construct_fromstring(smacq_environment * env, int type, void * data) {
  const dts_object * o = smacq_alloc(env, 0, type);
  //dts_incref(o, 1);
  if (smacq_fromstring(env, type, data, (dts_object*)o)) {
    return o;
  } else {
    dts_decref(o);
    return NULL;
  }
}

struct smacq_outputq {
	const dts_object * o;
	int outchan;
	struct smacq_outputq * next;
	struct smacq_outputq * end;
};

static inline void smacq_produce_enqueue(struct smacq_outputq ** qp, const dts_object * o, int outchan) {
  struct smacq_outputq * nq = malloc(sizeof(struct smacq_outputq));
  assert(qp);

  nq->o = o;
  nq->outchan = outchan;
  nq->next = NULL;

  if (!*qp) {
	  *qp = nq;
  } else {
	  (*qp)->end->next = nq;
  }

  (*qp)->end = nq;
}

static inline smacq_result smacq_produce_dequeue(struct smacq_outputq ** qp, const dts_object ** o, int * outchan) {
  struct smacq_outputq * head = *qp;

  if (!head) {
	  return SMACQ_FREE;
  }

  *o = head->o;
  *outchan = head->outchan;

  *qp = head->next;
  if (*qp)
  	(*qp)->end = head->end;

  free(head);

  if (*qp) {
	  return SMACQ_PASS|SMACQ_PRODUCE;
  } else {
	  return SMACQ_PASS;
  }
}

#endif

