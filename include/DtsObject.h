#ifndef DTSOBJECT_H
#define DTSOBJECT_H

#include <dts.h>
#include <assert.h>

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
/// reference counts or the user).
class DtsObject_ {

/// This macro casts a datum to a "type*" 
#define dts_data_as(datum,type) (*((type*)((datum)->getdata())))

/// This macro casts a datum to "type" and sets it to "val"
#define dts_set(datum,type,val) (datum)->setsize(sizeof(type)) , (*((type*)((datum)->getdata()))) = (val), 1 

  public:
	DtsObject_(DTS * dts, int size, int type);
	~DtsObject_();

	/// (Re-)initialize the object to the given size and type 
	void init(int size, dts_typeid type);

	/// Return a duplicate of the object
	DtsObject dup();

	/// @name Meta-data Methods
	/// @{
	void setsize(int size);
	int getsize() const { return(len); }

	unsigned long getid() const { return(id); }  

	unsigned char * getdata() const { return((unsigned char*)data); }

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
	int set_fromstring(char * datastr);
	/// @}
       
	/// @name Field Access
	/// @{
	DtsObject getfield(dts_field fieldv);
	void attach_field(dts_field field, DtsObject field_data);
	/// @}

	DtsObject make_writable();

	void prime_all_fields();

	int DtsObject_::write(struct pickle * pickle, int fd);

	void send(dts_field_element fieldnum, dts_comparison * comparisons) {
	  dts->send_message(this, fieldnum, comparisons);
	}

	void send(dts_field field, dts_comparison * comparisons) {
	  dts->send_message(this, field[0], comparisons);
	}

	friend DtsObject DTS::msg_check(DtsObject, dts_field_element);
	
	int match(dts_comparison * comps) { 
	  return match_andor(comps, AND); 
	}

	/// Expr module uses this
	double eval_arith_operand(struct dts_operand * op);

	//bool operator == (DtsObject_ &) const;

 private:
	/// @name Reference Counting
	/// Programmers should NOT use these methods directly
	/// @{

	/// Increment reference count by 1
	void incref();

	/// Decrement reference count by 1
	void decref();

	/// Used by DtsObject (boost::intrusive_ptr)
        friend void intrusive_ptr_add_ref(DtsObject_*o) { o->incref(); }

	/// Used by DtsObject (boost::intrusive_ptr)
        friend void intrusive_ptr_release(DtsObject_*o) { o->decref(); }

	/// Plain-C Wrapper 
	//friend void dts_decref(DtsObject d) { d->decref(); }


	/// @}
	
	
 private:
	/// Increment refcount by the given number
	void incref(int);

	//int getrefcount() const { return refcount; }

  	int refcount;

	/// Keep a reference to what we were derived from so it doesn't
 	/// get reused.
 	DtsObject uses;

	void freeObject();
	void fieldcache_flush(dts_field_element fnum);

	/// @name Matching
	/// @{
	void fetch_operand(struct dts_operand * op, dts_typeid const_type);
	int match_one(dts_comparison * c);
	int match_andor(dts_comparison * comps, int op);
	/// @}

	DtsObject getfield_single(dts_field_element fnum);
	DtsObject getfield_new(dts_field_element fnum);
	void attach_field_single(dts_field_element field, DtsObject field_data);

  	int free_data; /* boolean */

  	/* Cache of received messages */
        DynamicArray<DtsObject> fields;

  	/* data description */
  	int type;

  	void * data;
  	int len;

	void * buffer;
  	int buffer_size;

  	DTS * dts;

#ifndef SMACQ_OPT_NOPTHREADS
  	pthread_mutex_t mutex;
#endif

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
  fields.empty();
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
  : refcount(0), dts(dts)
{
#ifndef SMACQ_OPT_NOPTHREADS
  pthread_mutex_init(&this->mutex, NULL);
#endif

  this->buffer_size = 0;
  this->buffer = NULL;

  SMDEBUG(DtsObject_count++);
  SMDEBUG(fprintf(stderr, "%d objects currently allocated\n", DtsObject_count);)

  this->init(size, type);
}

inline int DtsObject_::set_fromstring(char * datastr) {
  struct dts_type * t = dts->type_bynum(this->type);

  if (!t) return 0;
  assert(t->info.fromstring);

  return t->info.fromstring(datastr, this);
}

inline void DtsObject_::incref(int i) {
#ifndef SMACQ_OPT_NOPTHREADS
  pthread_mutex_lock(&this->mutex);
#endif
  this->refcount += i;
#ifndef SMACQ_OPT_NOPTHREADS
  pthread_mutex_unlock(&this->mutex);
#endif
}

inline void DtsObject_::incref() {
#ifndef SMACQ_OPT_NOPTHREADS
  pthread_mutex_lock(&this->mutex);
#endif
  this->refcount++;
#ifndef SMACQ_OPT_NOPTHREADS
  pthread_mutex_unlock(&this->mutex);
#endif
}

inline void DtsObject_::decref() {
  //fprintf(stdout, "deref called for %p\n", d);

  DUMP_p(this);
  DUMP_d(this->refcount);
  assert(this->refcount > 0);

  DUMP();
#ifndef SMACQ_OPT_NOPTHREADS
  pthread_mutex_lock(&this->mutex);
#endif
  this->refcount--;
  if (!this->refcount) {
#ifndef SMACQ_OPT_NOPTHREADS
    pthread_mutex_unlock(&this->mutex);
#endif
  DUMP();

    this->freeObject();
  DUMP();
#ifndef SMACQ_OPT_NOPTHREADS
  } else {
    pthread_mutex_unlock(&this->mutex);
#endif
  }
}

inline void DtsObject_::attach_field(dts_field field, DtsObject field_data) {
  int fnum = dts_field_first(field);

  attach_field_single(fnum, field_data);

  assert(!dts_field_first(dts_field_next(field)));
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

