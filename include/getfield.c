#ifndef SMACQ_GETFIELD_C
#define SMACQ_GETFIELD_C

static inline const dts_object * dts_getfield_new(dts_environment * tenv, const dts_object * datum, dts_field_element fnum) {
  /* Construct a new object to return */
  int offset;
  struct dts_field_info * d;
  dts_object * field;
  struct dts_type * t = dts_type_bynum(tenv, dts_gettype(datum));
  assert(t);
  
  d = (struct dts_field_info*)darray_get(&t->fields, fnum);
  
  if (d) {
    int size = dts_type_bynum(tenv, d->type)->info.size;
    offset = d->offset;
    
    if (!d->desc.getfunc) {  
      assert(offset >= 0);
      //fprintf(stderr, "getfield has offset %d\n", offset);
      field = (dts_object*)dts_alloc(tenv, 0, d->type);
      field->len = size;
      field->data = (char*)datum->data + offset;
    } else {
      //fprintf(stderr, "getfield has helper func\n");
      field = (dts_object*)dts_alloc(tenv, size, d->type);
      if (!d->desc.getfunc(datum, field)) {
	/* getfunc failed, release memory */
	dts_decref(field);
	return NULL;
      }
    }
    return field;
    
  } else if (t->info.getfield) {
    field = (dts_object*)dts_alloc(tenv, 0, 0);
    
    if (t->info.getfield(datum, field, fnum)) {
      return field;
    } else {
      return NULL;
    }
    
  } else {
#ifndef SMACQ_OPT_NOMSGS
    return msg_check(tenv, datum, fnum);
#else
    return NULL;
#endif
  }
}

static inline const dts_object * dts_getfield_single(dts_environment * tenv, const dts_object * datum, dts_field_element fnum) {
  dts_object * cached;
  
  cached = (dts_object*)darray_get((struct darray*)&datum->fields, fnum);
  if (cached) {
    dts_incref(cached, 1);
    return cached;
  } else {
    const dts_object * field = dts_getfield_new(tenv, datum, fnum);
    
#ifdef SMACQ_OPT_FORCEFIELDCACHE
    if (field) {
      dts_incref(field, 1);
      darray_set(&((dts_object*)datum)->fields, fnum, field);
    }
#endif
    
    return field;
  }
}

static inline const dts_object* dts_getfield(dts_environment * tenv, const dts_object * datum, dts_field fieldv) {
  const dts_object * parent;
  const dts_object * f;

  parent = dts_getfield_single(tenv, datum, dts_field_first(fieldv));
  //fprintf(stderr, "Get field %d in %p from %p, next is %d\n", dts_field_first(fieldv), parent, datum, fieldv[1]);
  fieldv = dts_field_next(fieldv); 

  if (!dts_field_first(fieldv)) return parent;

  /* Must go deeper */
  while (1) {
    if (!parent) return parent;

    f = dts_getfield_single(tenv, parent, dts_field_first(fieldv));
    //fprintf(stderr, "Got field %d in %p from %p, next is %d\n", dts_field_first(fieldv), f, parent, fieldv[1]);

    /* Get rid of intermediate field */
    dts_decref(parent);

    fieldv = dts_field_next(fieldv); 
    if (!dts_field_first(fieldv) || !f)
      return f;

    parent = f;
  }
}

/* Get the named field from a datum */
static inline const dts_object * smacq_getfield(smacq_environment * env, const dts_object * datum, dts_field field, dts_object * data) {
  return(dts_getfield(env->types, datum, field));
}

#endif
