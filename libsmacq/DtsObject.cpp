#include <dts-internal.h>

#define MINSIZE 100

#ifdef SMACQ_DEBUG_MEM
# define SMDEBUG(x) x
#else
# define SMDEBUG(x)
#endif

static unsigned long Id = 0;

void DtsObject::init(int size, dts_typeid type) {
  this->type = type;
  this->free_data=0;
  this->refcount=1;
  this->id=Id++;
  this->data=this->buffer; 

  this->setsize(size);
}

void DtsObject::prime_all_fields() {
	int i;
	DtsObject * field;
  	struct dts_type * t = dts->type_bynum(this->type);

	for (i=0; i <= t->fields.max; i++) {
		if (darray_get(&t->fields, i)) {
			field = this->getfield_single(i);
			if (field) field->decref();
		}
	}
}

DtsObject * DtsObject::dup() {
	DtsObject * copy = dts->newObject(this->type, this->len);
	if (copy) 
		memcpy(copy->data, this->data, this->len);
	return copy;
}

void DtsObject::incref(int i) {
#ifndef SMACQ_OPT_NOPTHREADS
  pthread_mutex_lock(&this->mutex);
#endif
  this->refcount += i;
#ifndef SMACQ_OPT_NOPTHREADS
  pthread_mutex_unlock(&this->mutex);
#endif
}

void DtsObject::incref() {
#ifndef SMACQ_OPT_NOPTHREADS
  pthread_mutex_lock(&this->mutex);
#endif
  this->refcount++;
#ifndef SMACQ_OPT_NOPTHREADS
  pthread_mutex_unlock(&this->mutex);
#endif
}

void DtsObject::fieldcache_flush(dts_field_element fnum) {
  DtsObject * cached = (DtsObject*)darray_get((struct darray*)&this->fields, fnum);
  cached->decref(); //Can't inline this (recursive)
  darray_set((struct darray*)&this->fields, fnum, NULL);
  /*
  fprintf(stderr, "flushing child %p from cache of %p: %p\n", cached, this,
  			darray_get((struct darray*)&this->fields, fnum)
		  );
  */
}

void DtsObject::decref() {
  //fprintf(stdout, "deref called for %p\n", d);

  assert(this->refcount > 0);

#ifndef SMACQ_OPT_NOPTHREADS
  pthread_mutex_lock(&this->mutex);
#endif
  this->refcount--;
  if (!this->refcount) { 
#ifndef SMACQ_OPT_NOPTHREADS
    pthread_mutex_unlock(&this->mutex);
#endif

    this->freeObject();
#ifndef SMACQ_OPT_NOPTHREADS
  } else {
    pthread_mutex_unlock(&this->mutex);
#endif
  }
}

void DtsObject::attach_field(dts_field field, DtsObject * field_data) {
  int fnum = dts_field_first(field);

  attach_field_single(fnum, field_data);

  assert(!dts_field_first(dts_field_next(field)));
}

DtsObject * DtsObject::make_writable() {
  if (this->refcount > 1) {\
    return this->dup();
  } else {
    // We're the only user, so just modify this one
    this->incref();
    return this;
  }
}

#include <math.h>

double DtsObject::eval_arith_operand(struct dts_operand * op) {
  double val1, val2;

  switch (op->type) {
	  case FIELD:
  	  	 fetch_operand(op, -1);
		 if (op->valueo) {
		   assert(op->valueo->type == dts->typenum_byname("double"));
			 return dts_data_as(op->valueo, double);
		 } else {
			 fprintf(stderr, "Warning: no field %s to eval, using NaN for value\n", op->origin.literal.str);
			 return NAN;
		 };
		 break;

	  case CONST:
	    fetch_operand(op, dts->typenum_byname("double"));
		 if (op->valueo) {
			 return dts_data_as(op->valueo, double);
		 } else {
			 return NAN;
		 };
		 break;

	  case ARITH:
  		val1 = eval_arith_operand(op->origin.arith.op1);
  		val2 = eval_arith_operand(op->origin.arith.op2);
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

void DtsObject::attach_field_single(dts_field_element field, DtsObject * field_data) {
  DtsObject * old = (DtsObject*)darray_get( (struct darray *)&this->fields, field);
  //fprintf(stderr, "Attaching %p to %p field %d\n", field_data, this, field);
  darray_set( (struct darray *)&fields, field, (DtsObject*)field_data);
  if (old) old->decref();
}

void DtsObject::setdatacopy(void * src) {
  memcpy(data, src, len);
}

DtsObject * DtsObject::getfield_new(dts_field_element fnum) {
  /* Construct a new object to return */
  struct dts_field_info * d;
  DtsObject * field;
  struct dts_type * t = dts->type_bynum(this->gettype());
  assert(t);
  
  d = (struct dts_field_info*)darray_get(&t->fields, fnum);
  
  if (d) {
    field = dts->newObject(d->type);
    
    if (!d->desc.getfunc) {  
      int offset = d->offset;
      assert(offset >= 0);
      //fprintf(stderr, "getfield has offset %d\n", offset);
      field->data = (char*)this->data + offset;
    } else {
      //fprintf(stderr, "getfield has helper func\n");
      if (!d->desc.getfunc(this, field)) {
	/* getfunc failed, release memory */
	field->decref();
	return NULL;
      }
    }
    return field;
    
  } else if (t->info.getfield) {
    field = (DtsObject*)dts->newObject(t->num);
    
    if (t->info.getfield(this, field, fnum)) {
      return field;
    } else {
      return NULL;
    }
    
  } else {
#ifndef SMACQ_OPT_NOMSGS
    return dts->msg_check(this, fnum);
#else
    return NULL;
#endif
  }
}

DtsObject * DtsObject::getfield_single(dts_field_element fnum) {
  DtsObject * cached;
  
  cached = (DtsObject*)darray_get((struct darray*)&fields, fnum);
  if (cached) {
    cached->incref();
    return cached;
  } else {
    DtsObject * field = getfield_new(fnum);
    
#ifdef SMACQ_OPT_FORCEFIELDCACHE
    if (field) {
      field->incref();
      darray_set(&fields, fnum, field);
    }
#endif
    
    return field;
  }
}

DtsObject* DtsObject::getfield(dts_field fieldv) {
  DtsObject * parent;
  DtsObject * f;

  parent = getfield_single(dts_field_first(fieldv));
  //fprintf(stderr, "Get field %d in %p from %p, next is %d\n", dts_field_first(fieldv), parent, this, fieldv[1]);
  fieldv = dts_field_next(fieldv); 

  if (!dts_field_first(fieldv)) return parent;

  /* Must go deeper */
  while (1) {
    if (!parent) return parent;

    f = parent->getfield_single(dts_field_first(fieldv));
    //fprintf(stderr, "Got field %d in %p from %p, next is %d\n", dts_field_first(fieldv), f, parent, fieldv[1]);

    /* Get rid of intermediate field */
    parent->decref();

    fieldv = dts_field_next(fieldv); 
    if (!dts_field_first(fieldv) || !f)
      return f;

    parent = f;
  }
}

DtsObject::~DtsObject() {
  darray_free((struct darray *)(&this->fields));
  if (buffer) 
  	free(buffer);
  SMDEBUG(DtsObject_count--;)
}

void DtsObject::freeObject() {
  int i;

  // Flush field cache too then
  for (i=0; i<=this->fields.max; i++) {
    if (darray_get(&this->fields, i)) {
      this->fieldcache_flush(i);
    }
  }
  
  if (this->free_data) {
    //fprintf(stderr, "Freeing data %p of %p\n", this->data, d);
    free(this->data);
    this->data = NULL;
  }

#ifndef SMACQ_OPT_DTS_FREELIST
  delete this;
#else
  if (dts->freelist.p < dts->freelist.end) {
    	//fprintf(stderr, "DtsObject::free saving %p at %p between %p and %p\n", this, dts->freelist.p+1, dts->freelist.start, dts->freelist.end);
  	dts->freelist.p++;
  	*dts->freelist.p = this;
  } else {
    delete this;
  }
#endif
}

void DtsObject::setsize(int size) {
  if (size < 0) size = 0;

  if (this->buffer == this->data && (size > this->buffer_size)) {
    this->buffer = realloc(this->buffer, size);
    assert(this->buffer);
    this->buffer_size = size;
    this->data = this->buffer;
  }
  this->len = size;
}

#ifndef max
#define max(a,b) ((a)>(b) ? (a) : (b))
#endif

DtsObject::DtsObject(DTS * dts, int size, int type) : dts(dts) {
#ifndef SMACQ_OPT_NOPTHREADS
  pthread_mutex_init(&this->mutex, NULL);
#endif

  this->buffer_size = 0;
  this->buffer = NULL;
  darray_init(&this->fields, dts->maxfield());

  SMDEBUG(DtsObject_count++);
  SMDEBUG(fprintf(stderr, "%d objects currently allocated\n", DtsObject_count);)

  this->init(size, type);
}

int DtsObject::set_fromstring(char * datastr) {
  struct dts_type * t = dts->type_bynum(this->type);

  if (!t) return 0;
  assert(t->info.fromstring);

  return t->info.fromstring(datastr, this);
}

void DtsObject::fetch_operand(struct dts_operand * op, dts_typeid const_type) {
  if (op->type == CONST && op->valueo && op->valueo->type == const_type) {
	  return; 
  }

  if (op->type != ARITH && op->valueo) op->valueo->decref();

  switch(op->type) {
	case FIELD: 
	  op->valueo = this->getfield(op->origin.literal.field);
	  //if (!op->valueo) fprintf(stderr, "Field %s not found in obj %p\n", op->origin.literal.str, this);
	  break;

	case CONST: 
	  op->valueo = dts->construct_fromstring(const_type, op->origin.literal.str); 
	  if (!op->valueo) fprintf(stderr, "Could not parse %s\n", op->origin.literal.str);
	  break;

	case ARITH:
	  assert(op->valueo);
	  dts_data_as(op->valueo, double) = this->eval_arith_operand(op);
  	  //fprintf(stderr, "eval arithmetic operand to %g\n", dts_data_as(op->valueo, double));
	  break;
  }
}

SMDEBUG(static int DtsObject_count = 0;)
SMDEBUG(static int DtsObject_virtual_count = 0;)


int DtsObject::match_andor(dts_comparison * comps, int op) {
  dts_comparison * c;

  for (c = comps; c; c = c->next) {
	if (!this->match_one(c)) {
		//fprintf(stderr,"no (%d)\n", op);
		if (op == AND) 
		  	return 0;
	} else {
		//fprintf(stderr,"yes (%d)\n", op);
		if (op == OR)
			return 1;
	}
  }

  // fprintf(stderr, "total match\n");
  if (op == AND) 
	  return 1;
  else
	  return 0;
}

