#ifndef SMACQ_H
#define SMACQ_H
#include <gmodule.h>
#include <glib.h>
#include <sys/time.h>
#include <pthread.h>
#include <smacq_args.h>

#ifdef __cplusplus
#   define EXTERN extern "C"
#else
#   define EXTERN extern
#endif

typedef int smacq_result;

#define SMACQ_FREE 1
#define SMACQ_PASS 2
#define SMACQ_ERROR 4
#define SMACQ_END 8

#define SMACQ_CANPRODUCE 256
#define SMACQ_PRODUCE 512
#define SMACQ_MULTITHREAD 65536

typedef struct _dts_object dts_object;

struct _dts_object {
  /* private to engine */
  pthread_mutex_t mutex;
  int refcount;

  /* Cache of received messages */
  int numfields;
  const dts_object ** fields;

  /* data description */
  int type;
  void * data;
  int len;
};

typedef int field_getfunc_fn(const dts_object*, void ** data, int * len);

struct dts_field_descriptor {
  char * type;
  char * name;
  field_getfunc_fn * getfunc;
};

typedef int transform_getfunc_fn(void * data, int len, void ** tdata, int * tlen);

struct dts_transform_descriptor {
  char * name;
  transform_getfunc_fn * getfunc;
};

//typedef dts_environment struct _type_env;
//struct _type_env;
struct dts_type;

typedef struct _type_env {
  GHashTable * types_byname;
  GHashTable * fields_byname;
  GHashTable * messages_byfield;
  GHashTable * transform_names;
  int max_field;
  int max_type;
  int max_transform;
  
  struct dts_type ** types;

  int (* lt)(struct _type_env *, int, void *, int, void *, int);
  int (* fromstring)(struct _type_env *, int, char *, dts_object *);
  int (* getfield)(struct _type_env *, const dts_object * datum, int fnum, dts_object *);
  int (* presentdata)(struct _type_env *, dts_object *, int, void **, int*);
  int (* typenum_byname)(struct _type_env *, char *);
  int (* requiretype)(struct _type_env *, char *);
  int (* requirefield)(struct _type_env *, char *);
  char * (* typename_bynum)(struct _type_env *, int);
} dts_environment;


typedef struct _flow_env {
  dts_environment * types;

  const dts_object * (*alloc)(int, int);

} smacq_environment;

struct flow_init;

typedef smacq_result smacq_thread_fn(struct flow_init *);

struct flow_init {
  int isfirst;
  int islast;
  char ** argv;
  int argc;
  smacq_environment * env;
  void * state;
  struct filter * self;
  smacq_thread_fn * thread_fn;
};


/* 
 * Comparisons & filtering
 */
enum _dts_comp_op { EQUALITY, INEQUALITY, EXIST, LT, GT };
typedef enum _dts_comp_op dts_compare_operation;

typedef struct _dts_comparison {
  dts_compare_operation op;
  int field;
  dts_object field_data;

  char * valstr;

  int size;
  
  struct _dts_comparison * next;
} dts_comparison;

typedef struct _dts_msg {
  dts_object field_data;
  dts_comparison * criteria;
  struct _dts_msg * next;
} dts_message;

struct state; // Private to modules

typedef smacq_result flow_produce_fn(struct state * state, const dts_object **, int * outchan);
typedef smacq_result flow_consume_fn(struct state * state, const dts_object *, int * outchan);
typedef smacq_result flow_init_fn(struct flow_init *);
typedef smacq_result flow_shutdown_fn(struct state *);

/*
 * Interrogation structures
 */
struct smacq_functions {
  flow_produce_fn * produce;
  flow_consume_fn * consume;
  flow_init_fn * init;
  flow_shutdown_fn * shutdown;
  smacq_thread_fn * thread;
};

typedef int flowtype_lt_fn(void *, int, void *, int);
typedef int flowtype_fromstring_fn(char *, void **, int*);


struct dts_type_info {
  int size;
  flowtype_lt_fn * lt;
  flowtype_fromstring_fn * fromstring;
};



/*
 * Utility routines that have no shared state 
 */
EXTERN int flow_getoptsbyname(int argc, char ** argv, 
			      int * , char***, 
			      struct smacq_options *, 
			      struct smacq_optval *);

/*
 * Module threading routines 
 */

extern smacq_result smacq_thread_init(struct flow_init * volatile_context);
extern smacq_result smacq_thread_consume(struct state * state, const dts_object * datum, int * outchan);
extern smacq_result smacq_thread_produce(struct state * state, const dts_object ** datum, int *outchan);
extern smacq_result smacq_thread_shutdown(struct state * state);

extern const dts_object * smacq_read(struct flow_init * context);
extern void smacq_write(struct state * state, dts_object * datum, int outchan);
extern void smacq_decision(struct flow_init * context, const dts_object * datum, smacq_result result);
extern void  smacq_flush(struct flow_init * context);


/*
 * User Interface to main system
 */
struct filter;

enum flow_scheduler { ITERATIVE, RECURSIVE, THREADED, LOOP };
EXTERN int flow_start(struct filter *, enum flow_scheduler, dts_environment *);
void flow_init_modules(struct filter *, smacq_environment *);
EXTERN struct filter * smacq_build_pipeline(int argc, char ** argv);
struct filter * smacq_build_query(int argc, char ** argv);
struct filter * smacq_add_new_child(struct filter * parent, int argc, char ** argv);
int smacq_add_child(struct filter * parent, struct filter * newo);
struct filter * smacq_new_module(int argc, char ** argv);
EXTERN void smacq_free_module(struct filter * f);
struct filter * smacq_clone_child(struct filter * parent, int child);
struct filter * smacq_clone_tree(struct filter * donorParent, struct filter * newParent, int child);

#include <types-inline.c>

#endif

