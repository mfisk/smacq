#ifndef DTSOBJECT_H
#define DTSOBJECT_H

#include <dts.h>

#ifndef __cplusplus
typedef void DtsObject;
#else


class DtsObject{
  public:
	DtsObject(DTS * dts, int size, int type);
	~DtsObject();

	void init(int size, dts_typeid type);
	DtsObject* dup();

	void incref();
	void incref(int);
	void decref();

	void setsize(int size);
	int getsize() { return(len); }

	unsigned long getid() { return(id); }

	unsigned char * getdata() { return((unsigned char*)data); }
	void setdata(void * data) { this->data = data; }
	void setdatacopy(void * src);
	int set_fromstring(char * datastr);

	dts_typeid gettype() { return(type); }
	void settype(int type) { 
	  this->type = type; 
	  setsize(dts->type_bynum(type)->info.size);
	}

	DtsObject* getfield(dts_field fieldv);
	void attach_field(dts_field field, DtsObject * field_data);

	DtsObject * make_writable();

	void prime_all_fields();

	int DtsObject::write(struct pickle * pickle, int fd);

	/*
	 * Interfaces to msg system
	 */
	void send(dts_field_element fieldnum, dts_comparison * comparisons) {
	  dts->send_message(this, fieldnum, comparisons);
	}

	int match(dts_comparison * comps) { 
	  return match_andor(comps, AND); 
	}

	friend DtsObject * DTS::msg_check(DtsObject *, dts_field_element);
	struct iovec * fields2vec(struct fieldset * fieldset);

	/* Expr module uses this: */
	double eval_arith_operand(struct dts_operand * op);


  private:
	DtsObject ** fields2data(struct fieldset * fieldset);
	void freeObject();
	void fieldcache_flush(dts_field_element fnum);
	void fetch_operand(struct dts_operand * op, dts_typeid const_type);
	int match_one(dts_comparison * c);
	int match_andor(dts_comparison * comps, int op);

	DtsObject * getfield_single(dts_field_element fnum);
	DtsObject * getfield_new(dts_field_element fnum);
	void attach_field_single(dts_field_element field, DtsObject * field_data);

  	int refcount;

  	int free_data; /* boolean */

  	/* Cache of received messages */
  	struct darray fields; /* DtsObject * */

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

#endif
#endif

