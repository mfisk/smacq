#include <DtsObject.h>
#include <dts-module.h>
#include <dts.h>

#define DTS_FREELIST_SIZE 100

static unsigned long Id = 0;

void DtsObject_::init(int size, dts_typeid type) {
  this->type = type;
  this->free_data=0;
  this->id=Id++;
  this->data=this->buffer; 

  this->setsize(size);
}

void DtsObject_::prime_all_fields() {
	std::vector<struct dts_field_info*>::iterator i;
	DtsObject field;
  	dts_type * t = dts->type_bynum(this->type);

	for (i=t->fields.begin(); i != t->fields.end(); i++) {
		/* Get and release immediately */
		getfield_single((*i)->elementid);
	}
}

DtsObject DtsObject_::dup() {
	DtsObject copy = dts->newObject(this->type, this->len);
	if (copy) 
		memcpy(copy->data, this->data, this->len);
	return copy;
}

void DtsObject_::fieldcache_flush(dts_field_element fnum) {
  this->fields[fnum] = NULL;
}

DtsObject DtsObject_::make_writable() {
  if (this->refcount > 1) {
    return this->dup();
  } else {
    // We're the only user, so just modify this one
    return this;
  }
}

#include <math.h>

double DtsObject_::eval_arith_operand(struct dts_operand * op) {
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

DtsObject DtsObject_::getfield_new(dts_field_element fnum) {
  /* Construct a new object to return */
  DtsObject field;
  struct dts_type * t = dts->type_bynum(this->gettype());
  assert(t);
 
  dts_field_info * d = t->fields[fnum];
  
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
	return NULL;
      }
    }
    field->uses = this;
    return field;
    
  } else if (t->info.getfield) {
    field = (DtsObject)dts->newObject(t->num);
    field->uses = this;
    
    if (!t->info.getfield(this, field, fnum)) {
      return NULL;
    }
    
  } else {
#ifndef SMACQ_OPT_NOMSGS
    return dts->msg_check(this, fnum);
#else
    return NULL;
#endif
  }
  return field;
}

DtsObject DtsObject_::getfield_single(dts_field_element fnum) {
  DtsObject cached;
  
  cached = fields[fnum];
  if (cached) {
    //cached->incref(); // now auto
    return cached;
  } else {
    DtsObject field = getfield_new(fnum);
    
#ifdef SMACQ_OPT_FORCEFIELDCACHE
    if (field) {
      //field->incref(); // now auto
      fields[fnum] = field;
    }
#endif
    
    return field;
  }
}

DtsObject DtsObject_::getfield(dts_field fieldv) {
  DtsObject parent;
  DtsObject f;

  parent = getfield_single(dts_field_first(fieldv));
  //fprintf(stderr, "Get field %d in %p from %p, next is %d\n", dts_field_first(fieldv), parent, this, fieldv[1]);
  fieldv = dts_field_next(fieldv); 

  if (!dts_field_first(fieldv)) return parent;

  /* Must go deeper */
  while (1) {
    if (!parent) return parent;

    f = parent->getfield_single(dts_field_first(fieldv));
    //fprintf(stderr, "Got field %d in %p from %p, next is %d\n", dts_field_first(fieldv), f, parent, fieldv[1]);

    fieldv = dts_field_next(fieldv); 
    if (!dts_field_first(fieldv) || !f)
      return f;

    parent = f;
  }
}

void DtsObject_::freeObject() {
  if (this->free_data) {
    //fprintf(stderr, "Freeing data %p of %p\n", this->data, d);
    free(this->data);
    this->data = NULL;
  }

#ifndef SMACQ_OPT_DTS_FREELIST
  delete this;
#else

  if (dts->freelist.size() < DTS_FREELIST_SIZE) {
  	// Flush field cache too then
	fields.empty();
  
	dts->freelist.push(this);
  } else {
    delete this;
  }
#endif
}

#ifndef max
#define max(a,b) ((a)>(b) ? (a) : (b))
#endif

void DtsObject_::fetch_operand(struct dts_operand * op, dts_typeid const_type) {
  if (op->type == CONST && op->valueo && op->valueo->type == const_type) {
	  return; 
  }

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


int DtsObject_::match_andor(dts_comparison * comps, int op) {
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

