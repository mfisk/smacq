#ifndef DTS_H
#define DTS_H

//struct dts_type;

typedef struct _dts_object dts_object;
typedef unsigned short dts_field_element;
typedef dts_field_element * dts_field;
typedef int field_getfunc_fn(const dts_object*, dts_object*);

struct dts_freelist {
  const dts_object ** start;
  const dts_object ** p;
  const dts_object ** end;
};

typedef struct _type_env {
  GHashTable * types_byname;
  GHashTable * fields_byname;
  int max_type;
  int max_field;
  struct dts_freelist freelist;

  struct darray messages_byfield;
  struct darray types; /* struct dts_type * */
  struct darray fields_bynum; /* char * */

  int (* lt)(struct _type_env *, int, void *, int, void *, int);
  int (* fromstring)(struct _type_env *, int, char *, dts_object *);
  const dts_object * (* getfield)(struct _type_env *, const dts_object * datum, dts_field fnum, dts_object *);
  int (* typenum_byname)(struct _type_env *, char *);
  int (* requiretype)(struct _type_env *, char *);
  dts_field (* requirefield)(struct _type_env *, char *);
  char * (* typename_bynum)(struct _type_env *, int);
} dts_environment;

struct _dts_object {
  /* private to engine */
  int refcount;

  int free_data; /* boolean */

  /* Cache of received messages */
  struct darray fields; /* const dts_object * */

  /* data description */
  int type;
  void * data;
  int len;
  int max_size;

  dts_environment * tenv;

#ifndef SMACQ_OPT_NOPTHREADS
  pthread_mutex_t mutex;
#endif
};

struct dts_field_spec {
  char * type;
  char * name;
  field_getfunc_fn * getfunc;
};

struct dts_field_info {
  int type;
  int offset;
  struct dts_field_spec desc;
};

typedef int smacqtype_lt_fn(void *, int, void *, int);
typedef int smacqtype_fromstring_fn(char *, const dts_object *);
typedef int smacqtype_getfield_fn(const dts_object *, dts_object *, dts_field_element);

struct dts_type_info {
  int size;
  smacqtype_lt_fn * lt;
  smacqtype_fromstring_fn * fromstring;
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

/* 
 * Comparisons & filtering
 */
enum _dts_comp_op { EXIST, EQ, LEQ, GEQ, NEQ, LT, GT, LIKE, AND, OR, FUNC };
typedef enum _dts_comp_op dts_compare_operation;

typedef struct _dts_comparison {
  dts_compare_operation op;
  dts_field field;
  dts_object field_data;

  char * fieldname;
  char * valstr;
  struct arglist * arglist;
  int size;
  struct _dts_comparison * next;

  int argc;
  char ** argv;

  struct _dts_comparison * group;
} dts_comparison;

typedef struct _dts_msg {
  dts_object field_data;
  dts_comparison * criteria;
  struct _dts_msg * next;
} dts_message;

void msg_send(dts_environment *, dts_field_element, dts_object *, dts_comparison *);
dts_object * dts_construct(dts_environment * tenv, int type, void * data);
static void dts_decref(const dts_object * d_const);

#define dts_field_first(x) (x[0])
int dts_comparefields(dts_field a, dts_field b);

const dts_object* dts_alloc(dts_environment * tenv, int size, int type);

#define dts_data_as(datum,type) (*((type*)((datum)->data)))
#define dts_set(datum,type,val) dts_setsize((datum), sizeof(type)) , (*((type*)((datum)->data))) = (val), 1 
	
/*
 * Interface to data testing system
 */
int dts_parsetest(dts_environment * tenv, dts_comparison * comp, char * test);
int type_match(dts_environment * tenv, const dts_object * datum, 
	       dts_comparison * comps, int same_types);

dts_comparison * dts_parse_tests(dts_environment * tenv, int argc, char ** argv);
char * dts_field_getname(dts_environment * tenv, dts_field f);
void dts_free(const dts_object*);


#endif
