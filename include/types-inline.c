#ifndef SMACQ_INLINE_C
#define SMACQ_INLINE_C

#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "util.c"
#include "darray.c"
#include "produceq.c"

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

static inline void smacq_msg_send(smacq_environment * env, dts_field field, const dts_object * d, dts_comparison * comps) {
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

static inline int dts_setsize(const dts_object * cd, int size) {
	dts_object * d = (dts_object*)cd;
	if (size > d->max_size) {
		d->data = malloc(size);
		d->free_data = 1;
	}
	d->len = size;

	return 1;
}

/* Convert a type number to a type name */
static inline char * dts_typename_bynum(smacq_environment * env, int num) {
  return(env->types->typename_bynum(env->types, num));
}

/* Convert a type name to a type number */
static inline int smacq_typenum_byname(smacq_environment * env, char * name) {
  return(env->types->typenum_byname(env->types, name));
}

static inline char * dts_fieldname_append(const char * old, const char * new) {
  char * ret = malloc(strlen(old) + strlen(new) + 2);
  sprintf(ret, "%s.%s", old, new);
  return ret;
}

static inline int smacq_match(smacq_environment * env, 
			     const dts_object * datum, 
			     dts_comparison * comps) {
  return(type_match(env->types, datum, comps));
}

static inline struct dts_type * dts_type_bynum(dts_environment * tenv, int num) {
  return darray_get(&tenv->types, num);
}

static inline int dts_type_size(dts_environment * tenv, int type) {
  struct dts_type * t = dts_type_bynum(tenv, type);
  if (!t) return -2;
  return t->info.size;
}

static inline const dts_object * dts_construct_fromstring(dts_environment * tenv, int type, void * data) {
  const dts_object * o = dts_alloc(tenv, 0, type);
  //dts_incref(o, 1);
  if (tenv->fromstring(tenv, type, data, (dts_object*)o)) {
    return o;
  } else {
    dts_decref(o);
    return NULL;
  }
}

#include "getfield.c"

static double eval_arith_operand(const dts_object * datum, struct dts_operand * op);

static inline void fetch_operand(dts_environment * tenv, const dts_object * datum, 
	   struct dts_operand * op, int const_type) {
  if (op->type == CONST && op->valueo && op->valueo->type == const_type) {
	  return; 
  }

  if (op->type != ARITH && op->valueo) dts_decref(op->valueo);

  switch(op->type) {
	case FIELD: 
	  op->valueo = dts_getfield(tenv, datum, op->origin.literal.field);
	  //if (!op->valueo) fprintf(stderr, "Field %s not found in obj %p\n", op->origin.literal.str, datum);
	  break;

	case CONST: 
	  op->valueo = dts_construct_fromstring(tenv, const_type, op->origin.literal.str); 
	  if (!op->valueo) fprintf(stderr, "Could not parse %s\n", op->origin.literal.str);
	  break;

	case ARITH:
	  assert(op->valueo);
	  dts_data_as(op->valueo, double) = eval_arith_operand(datum, op);
  	  //fprintf(stderr, "eval arithmetic operand to %g\n", dts_data_as(op->valueo, double));
	  break;
  }
}

static double eval_arith_operand(const dts_object * datum, struct dts_operand * op) {
  double val1, val2;

  switch (op->type) {
	  case FIELD:
  	  	 fetch_operand(datum->tenv, datum, op, -1);
		 if (op->valueo) {
		 	 assert(op->valueo->type == datum->tenv->double_type);
			 return dts_data_as(op->valueo, double);
		 } else {
			 fprintf(stderr, "Warning: no field %s to eval, using NaN for value\n", op->origin.literal.str);
			 return NAN;
		 };
		 break;

	  case CONST:
  	  	 fetch_operand(datum->tenv, datum, op, datum->tenv->double_type);
		 if (op->valueo) {
			 return dts_data_as(op->valueo, double);
		 } else {
			 return NAN;
		 };
		 break;

	  case ARITH:
  		val1 = eval_arith_operand(datum, op->origin.arith.op1);
  		val2 = eval_arith_operand(datum, op->origin.arith.op2);
  		switch (op->origin.arith.type) {
	  		case ADD:
			  //fprintf(stderr, "%g + %g\n", val1, val2);
		  		return val1 + val2;
		  		break;
	  		case SUB:
			  //fprintf(stderr, "%g - %g\n", val1, val2);
		  		return val1 - val2;
		  		break;
	  		case DIVIDE:
			  //fprintf(stderr, "%g / %g\n", val1, val2);
		  		return val1 / val2;
		  		break;
	  		case MULT:
			  //fprintf(stderr, "%g * %g\n", val1, val2);
	  	  		return val1 * val2;
		  		break;
  		}
		break;
  }
  return NAN;
}



#endif
