#ifndef DTS_H
#define DTS_H

#include <smacq.h>
#include <gmodule.h>
#include <dts-types.h>
#include <dts-filter.h>

#ifndef __cplusplus
#warning "C++ is required when including Dts.h"
#endif 

static inline void dts_field_free(dts_field field) {
  return free(field);
}

static inline dts_field dts_field_next(dts_field field) {
  if (field[0])
  	return field+1;
  else
	return NULL;
}

static inline char * dts_fieldname_append(const char * old, const char * newf) {
  char * ret = (char*)malloc(strlen(old) + strlen(newf) + 2);
  sprintf(ret, "%s.%s", old, newf);
  return ret;
}

struct dts_freelist {
  DtsObject ** start;
  DtsObject ** p;
  DtsObject ** end;
};

typedef int smacqtype_lt_fn(void *, int, void *, int);
typedef int smacqtype_fromstring_fn(char *, DtsObject *);
typedef int smacqtype_getfield_fn(DtsObject *, DtsObject *, dts_field_element);

struct dts_type_info {
  int size;
  smacqtype_fromstring_fn * fromstring;
  smacqtype_lt_fn * lt;
  smacqtype_getfield_fn * getfield;
};

struct dts_type {
  struct darray fields;
  char * name;
  int num;
  GModule * module;
  struct dts_field_spec * description;
  struct dts_type_info info;
};

typedef int dts_typeid;

class DTS {
 public:
  DTS();
  
  char * typename_bynum(dts_typeid);
  
  struct dts_type * type_bynum(dts_typeid id) {
    return (struct dts_type*)darray_get(&types, id);
  }
  
  dts_typeid typenum_byname(char * name);
  
  DtsObject * construct(dts_typeid, void * data);
  DtsObject * construct_fromstring(dts_typeid type, char * data);
  DtsObject * newObject(dts_typeid);
  DtsObject * newObject(dts_typeid, int size);
  DtsObject * readObject(struct pickle * pickle, int fd);

  int fromstring(dts_typeid, char * datastr, DtsObject * data);
  
  int getfieldoffset(DtsObject * datum, int fnum, int * dtype, int * offset, int * len);
  char * field_getname(dts_field f);
  
  int dts_lt(int type, void * p1, int len1, void * p2, int len2);
  dts_field requirefield(char * name);
  dts_typeid requiretype(char * name);
  
  /*
   * Interface to data testing system
   */
  int parsetest(dts_comparison * comp, char * test);
  int match(DtsObject * datum, dts_comparison * comps);
  
  dts_comparison * parse_tests(int argc, char ** argv);
  struct dts_operand * parse_expr(int argc, char ** argv);
  
  struct dts_freelist freelist;
  int maxfield() { return max_field; }
  
  void send_message(DtsObject *, dts_field_element, dts_comparison *);
  DtsObject * msg_check(DtsObject * o, dts_field_element field);

  void fields_init(struct fieldset * fieldset, int argc, char ** argv);

  int type_size(dts_typeid type) {
    struct dts_type * t = this->type_bynum(type);
    if (!t) return -2;
    return t->info.size;
  }

 private:
  GHashTable * types_byname;
  GHashTable * fields_byname; 
  int max_type; 
  int max_field; 
  struct darray messages_byfield;
  
  struct darray types; /* struct dts_type * */
  struct darray fields_bynum; /* char * */
  
  //  	dts_field double_field;
  //  	int double_type;
  
  dts_field_element requirefield_single(char * name);
  

  int pickle_getnewtype(int fd, struct sockdatum * hdr, struct pickle * pickle);
  int pickle_addnewtype(char * name, int extnum, struct pickle * pickle, int fd);
  void print_comp(dts_comparison * c);

  struct dts_operand * comp_arith(enum dts_arith_operand_type op, struct dts_operand * op1, struct dts_operand * op2);
  void make_fields_doubles(struct dts_operand * operand);

  friend int yysmacql_parse();
  friend int yyfilterparse();
  friend int yyexprparse();
};

BEGIN_C_DECLS

#define dts_field_first(x) (x[0])
int dts_comparefields(dts_field a, dts_field b);

#define dts_data_as(datum,type) (*((type*)((datum)->getdata())))
#define dts_set(datum,type,val) (datum)->setsize(sizeof(type)) , (*((type*)((datum)->getdata()))) = (val), 1 

void dts_decref(DtsObject * d);
	
typedef void smacq_filter_callback_fn(char * operation, int argc, char ** argv, void * data);
void smacq_downstream_filters(smacq_graph * mod, smacq_filter_callback_fn callback, void * data);
END_C_DECLS

#include <DtsObject.h>

#endif

