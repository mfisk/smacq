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

/// Return a new DtsObject that shares the underlying data, but with its own 
/// fieldspace.
DtsObject DtsObject_::private_copy() {
	DtsObject copy = dts->newObject(this->type, 0);
	copy->fields = this->fields;
	copy->setdata(this->getdata());
	copy->uses = this;
	return copy;
}

/// Shallow copy.  Preserve all attached fields.
DtsObject DtsObject_::dup() {
	DtsObject copy = dts->newObject(this->type, this->len);
	if (copy) {
		memcpy(copy->data, this->data, this->len);
		copy->fields = this->fields;
	}
	return copy;
}

void DtsObject_::fieldcache_flush(dts_field_element fnum) {
  this->fields[fnum] = NULL;
}

DtsObject DtsObject_::make_writable() {
  if (this->refcount.get() > 1) {
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
		 fetch_operand(op);
		 if (op->valueo) {
			//fprintf(stderr, "eval got operand obj %p type %s\n", op->valueo.get(), op->valueo->gettypename());
		   	if (op->valueo->type != dts->typenum_byname("double")) {
				op->valueo = op->valueo->getfield("double");
			}
			//fprintf(stderr, "eval got operand obj %p type %s value %g\n", op->valueo.get(), op->valueo->gettypename(), dts_data_as(op->valueo, double));
  		   	return dts_data_as(op->valueo, double);
		 } else {
			fprintf(stderr, "Warning: no field %s to eval, using NaN for value\n", op->origin.literal.str);
			return NAN;
		 };
		 break;

	  case CONST:
	    	fetch_const_operand(op, dts->typenum_byname("double"));
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
			  //fprintf(stderr, "%p: %g(%p) * %p: %g(%p)\n", op->origin.arith.op1, val1, op->origin.arith.op1->valueo.get(), op->origin.arith.op2, val2, op->origin.arith.op2->valueo.get());
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
    usecount.increment();
    return field;

  } else if (t->info.getfield) {
    field = dts->newObject(t->num);
    field->uses = this;
    usecount.increment();

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
    return cached;
  } else {
    /// There could be a race where multiple threads are
    /// creating the same field, but reference counting
    /// will cause and redunandant ones to go away.
    DtsObject field = getfield_new(fnum);

#ifndef SMACQ_OPT_NOFIELDCACHE
    if (field) {
	  //fprintf(stderr, "caching ptr %p\n", field.get());
      fields[fnum] = field;
    }
#endif

    return field;
  }
}

DtsObject DtsObject_::getfield(DtsField &fieldv, bool nowarn) {
  DtsField::iterator i;
  DtsObject f = this;

  for (i = fieldv.begin(); i != fieldv.end(); ++i) {
	f = f->getfield_single(*i);
	if (!f) {
    		if (dts->warn_missing_fields() && !nowarn) {
			std::string fieldname = dts->field_getname(fieldv);
			// This is an ugly exclusion (see uniqobj module):
			fprintf(stderr, "Warning: requested field %s not present\n", fieldname.c_str());
		}

		return f;
	}
  }

  return f;
}

void DtsObject_::prime_field(dts_field_info* f) {
	if (f) { // Field vectors are sparse, so we may get called with a lot of NULLs
		getfield_single(f->elementid);
	}
}

void DtsObject_::prime_all_fields() {
	using namespace boost::lambda;

	dts_type * t = dts->type_bynum(this->type);
	t->fields.foreach( bind(&DtsObject_::prime_field, this, _1) );
}

void DtsObject_::freeObject() {
  if (this->free_data) {
    //fprintf(stderr, "Freeing data %p of %p\n", this->data, d);
    free(this->data);
    this->data = NULL;
  }
  if (this->uses) {
	this->uses->usecount.decrement();
	this->uses = NULL;
  }

#ifndef SMACQ_OPT_DTS_FREELIST
  delete this;
#else

  if (dts->freelist.size() < DTS_FREELIST_SIZE) {
	// Flush field cache too then
	fields.clear();

	dts->freelist.push(this);
  } else {
    delete this;
  }
#endif
}

#ifndef max
#define max(a,b) ((a)>(b) ? (a) : (b))
#endif

void DtsObject_::fetch_const_operand(struct dts_operand * op, dts_typeid const_type) {
  	if (op->type == CONST && op->valueo && op->valueo->type == const_type) {
	  return;
  	}

	op->valueo = dts->construct_fromstring(const_type, op->origin.literal.str);
	if (!op->valueo) fprintf(stderr, "Could not parse %s\n", op->origin.literal.str);
}

void DtsObject_::fetch_operand(struct dts_operand * op, bool nowarn) {
  switch(op->type) {
	case FIELD:
	  op->valueo = this->getfield(op->origin.literal.field, nowarn);
	  //fprintf(stderr, "fetched operand %p %d\n", op->valueo.get(), op->origin.literal.field[0]);
	  //if (!op->valueo) fprintf(stderr, "Field %s not found in obj %p\n", op->origin.literal.str, this);
	  break;

	case CONST:
	  assert(0);
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
