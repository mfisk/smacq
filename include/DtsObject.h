#ifndef DTSOBJECT_H
#define DTSOBJECT_H

#include <dts.h>
#include <assert.h>
#include <ThreadSafe.h>
#include <string>
//#include <python/Python.h>

#include "dump.h"

#ifndef __cplusplus
typedef void * DtsObject;
#else

#ifdef SMACQ_DEBUG_MEM
# define SMDEBUG(x) x
#else
# define SMDEBUG(x)
#endif

SMDEBUG(static int DtsObject_count = 0;)
SMDEBUG(static int DtsObject_virtual_count = 0;)

/*!

\class DtsObject
\brief A DtsObject is an auto-pointer to a DtsObject_ instance

*/

/// DtsObject_ instances should only be used via
/// DtsObject auto-pointers (the auto-pointer keeps track of
/// reference counts for the user).
///
/// An object is read-only except for initializaton (when it is
/// assumed to only have a single user and therefore not require
/// locking).  Only the field cache is locked for thread safety.
///
/// A DtsObject is mainly accessed by requesting one of its fields 
/// with the getfield() method.  Most objects will have
/// a "string" and/or "double" field for accessing string or double 
/// representations of the object (if applicable).  
/// 
/// To access the raw contents of an object, use the getdata() 
/// and getsize() methods or the dts_data_as(object, type) macro
/// which will cast the data to the given type.
///
/// All DtsObjects have an underlying type that defines 0 or more fields
/// that can be extracted from that object.  The gettype() method
/// returns the dts_typeid.  However, a DtsObject may have additional fields
/// added to it at runtime with the attach_field() method.  To get
/// a list of all of the current fields, you can use the fieldcache()
/// method.  To ensure that the fieldcache contains all available fields for the underlying
/// type, precede the fieldcache() call with a call to prime_all_fields()

class DtsObject_ : public PthreadMutex {

/// This macro casts a datum to a "type*"
#define dts_data_as(datum,type) (*((type*)((datum)->getdata())))

/// This macro casts a datum to "type" and sets it to "val"
#define dts_set(datum,type,val) (datum)->setsize(sizeof(type)), (*((type*)((datum)->getdata()))) = (val), true

  public:
	DtsObject_(DTS * dts, int size, int type);
	~DtsObject_();

	/// (Re-)initialize the object to the given size and type
	void init(int size, dts_typeid type);

	/// @name Copy Constructors
	/// @{

	/// Return a new object with a copy of the data and a private field vector.
	/// The field vector is a copy of the original.
	DtsObject dup();

	/// Return a new object with shared data, but a private field vector.
	/// The field vector is a copy of the original.
	DtsObject private_copy();

	/// @}
	

	/// @name Meta-data Methods
	/// @{
	void setsize(int size);
	int getsize() const { return(len); }

	/// Return the unique numeric identifier of the object
	unsigned long getid() const { return(id); }

	/// Return the raw data contents of the object
	unsigned char * getdata() const { return((unsigned char*)data); }

	/// Return the raw data contents as a C++ string
        std::string pygetdata() const { 
           return std::string((char*)data, len) + '\0';
        }

	/// Return the type name of the object
	const char * gettypename() const { return dts->typename_bynum(type); }

	/// Return the type of the object
	dts_typeid gettype() const { return(type); }

	void settype(int type) {
	  this->type = type;
	  setsize(dts->type_bynum(type)->info.size);
	}
	/// @}

	/// @name Initializers
	/// @{
	void setdata(void * data) { this->data = data; }
	void setdatacopy(const void * src);
	int set_fromstring(const char * datastr);
	/// @}

	/// @name Field Access
	/// @{
		/// Return a field object
	DtsObject getfield(DtsField &fieldv, bool nowarn = false);

		/// Less efficient lookup by string
	DtsObject getfield(const char * s, bool nowarn = false) {
		DtsField f = dts->requirefield(s);
		return getfield(f, nowarn);
	}

	/// Attach an object as a field
	void attach_field(DtsField &field, DtsObject field_data);
	/// @}

	DtsObject make_writable();

	/// Instantiate all fields defined by the type of this object
	void prime_all_fields();

	/// Instantiate a specific field
	void prime_field(dts_field_info*);

	/// Get a copy of all instantiated fields.
	/// You may want to use prime_all_fields() first.
    std::vector<DtsObject> fieldcache() {
	   return fields.snapshot();
	}

	int write(struct pickle * pickle, int fd);

	void send(dts_field_element fieldnum, dts_comparison * comparisons) {
	  dts->send_message(this, fieldnum, comparisons);
	}

	void send(DtsField &field, dts_comparison * comparisons) {
	  dts->send_message(this, field[0], comparisons);
	}

	friend DtsObject DTS::msg_check(DtsObject, dts_field_element);

	int match(dts_comparison * comps) {
	  return match_andor(comps, AND);
	}

	/// Expr module uses this
	double eval_arith_operand(struct dts_operand * op);

	//bool operator == (DtsObject_ &) const;

	/// Pointer to the DTS used by this type
	DTS * getDts() const { return dts; }

	/// Return field name
	std::string getfieldname(dts_field_element f) { return std::string(dts->field_getname(f)); }

 private:
	/// Pointer to type system for this objecT
 	DTS * dts;

	/// @name Reference Counting
	/// Programmers should NOT use these methods directly
	/// @{

	/// Increment reference count
	void incref();

	/// Decrement reference count by 1
	void decref();

	/// Used by DtsObject (boost::intrusive_ptr)
        friend void intrusive_ptr_add_ref(DtsObject_*o) { 
		o->incref(); 
	}

	/// Used by DtsObject (boost::intrusive_ptr)
        friend void intrusive_ptr_release(DtsObject_*o) { 
		o->decref(); 
	}

	/// Plain-C Wrapper
	//friend void dts_decref(DtsObject d) { d->decref(); }

	/// @}


	ThreadSafeCounter refcount, usecount;

	/// Keep a reference to what we were derived from so it doesn't
	/// get reused.
	DtsObject uses;

	void freeObject();
	void fieldcache_flush(dts_field_element fnum);

	/// @name Matching
	/// @{
	void fetch_operand(struct dts_operand * op, bool nowarn = false);
	void fetch_const_operand(struct dts_operand * op, dts_typeid const_type);
	int match_one(dts_comparison * c);
	int match_andor(dts_comparison * comps, int op);
	/// @}

	DtsObject getfield_single(dts_field_element fnum);
	DtsObject getfield_new(dts_field_element fnum);
	void attach_field_single(dts_field_element field, DtsObject field_data);

	int free_data; /* boolean */

	/* Cache of received messages */
    ThreadSafeDynamicArray<DtsObject> fields;

	/* data description */
	int type;

	void * data;
	int len;

	void * buffer;
	int buffer_size;

	unsigned long id;
};

inline void DtsObject_::attach_field_single(dts_field_element field, DtsObject field_data) {
  //fprintf(stderr, "Attaching %p to %p field %d\n", field_data, this, field);
  fields[field] = field_data;
}

inline void DtsObject_::setdatacopy(const void * src) {
  memcpy(data, src, len);
}

inline DtsObject_::~DtsObject_() {
  fields.clear();
  if (buffer)
        free(buffer);
  SMDEBUG(DtsObject_count--;)
}

inline void DtsObject_::setsize(int size) {
  if (size < 0) size = 0;

  if (this->buffer == this->data && (size > this->buffer_size)) {
    this->buffer = realloc(this->buffer, size);
    assert(this->buffer);
    this->buffer_size = size;
    this->data = this->buffer;
  }
  this->len = size;
}

inline DtsObject_::DtsObject_(DTS * dts, int size, int type)
  : dts(dts)
{
  this->buffer_size = 0;
  this->buffer = NULL;

  SMDEBUG(DtsObject_count++);
  SMDEBUG(fprintf(stderr, "%d objects currently allocated\n", DtsObject_count);)

  this->init(size, type);
}

inline int DtsObject_::set_fromstring(const char * datastr) {
  struct dts_type * t = dts->type_bynum(this->type);

  if (!t) return 0;
  assert(t->info.fromstring);

  return t->info.fromstring(datastr, this);
}

inline void DtsObject_::incref() {
  refcount.increment();
}

inline void DtsObject_::decref() {
  //DUMP_p(this);
  int r = refcount.decrement();
  assert(r >= 0);

  //fprintf(stderr, "%p now %d, %d\n", this, refcount, usecount);

  if (r && r == usecount.get()) {
    /* All of the things that refer to us have us as a "uses".
	   Flush our fieldcache sense it's unnecessary.  
	   This will probably remove some or all of the uses references
	   and may get our refcount down to 0
     */
	//fprintf(stderr, "%p, cycle of %d ", this, usecount);

	/* Increment the refcount so that if when clear() recurses into decref()
	   we don't destroy ourselves there.  We'll decrement
	   it back to reality when we're done.
     */
	refcount.increment();
	fields.clear();
	r = refcount.decrement();
  }

  if (!r) {
    freeObject();
  }
}

inline void DtsObject_::attach_field(DtsField &field, DtsObject field_data) {
  assert(field.size() == 1);
  attach_field_single(field[0], field_data);
}


inline bool operator< (DtsObject_&a, DtsObject_&b) {
  if ( (a.getsize() < b.getsize()) ||
       (memcmp(a.getdata(), b.getdata(), a.getsize()) < 0)) {
    return true;
  } else {
    return false;
  }
}

inline bool operator== (DtsObject_&a, DtsObject_&b) {
  if ( (a.getsize() == b.getsize()) &&
       (!memcmp(a.getdata(), b.getdata(), a.getsize()))) {
    //fprintf(stderr, "DtsObject %p == %p\n", &a, &b);
    return true;
  } else {
    //fprintf(stderr, "DtsObject %p != %p\n", &b, &b);
    return false;
  }
}

#endif
#endif
