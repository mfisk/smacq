#ifndef SMACQ_H
#define SMACQ_H

/* Turn on performance optimizations: */
#define SMACQ_OPT_NOPTHREADS
#define SMACQ_OPT_NOMSGS
#define SMACQ_OPT_FORCEFIELDCACHE
#define SMACQ_OPT_VECTORS
#define SMACQ_OPT_RUNRING

#include <stdio.h>
#include <gmodule.h>
#include <glib.h>
#include <sys/time.h>

#ifndef SMACQ_OPT_NOPTHREADS
#include <pthread.h>
#else
#define pthread_mutex_init(x,y)
#define pthread_mutex_destroy(x)
#define pthread_mutex_lock(x)
#define pthread_mutex_unlock(x)

#define pthread_cond_init(x,y)
#define pthread_cond_destroy(x)
#define pthread_cond_wait(x,y)
#define pthread_cond_broadcast(x)
#define _PTHREAD_H
#endif

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
typedef unsigned short dts_field_element;
typedef dts_field_element * dts_field;
struct _smacq_module;
typedef struct _smacq_module smacq_graph;

struct darray {
  unsigned long * array;
  int max;
};

typedef int field_getfunc_fn(const dts_object*, dts_object*);

struct dts_field_descriptor {
  char * type;
  char * name;
  field_getfunc_fn * getfunc;
};

//typedef dts_environment struct _type_env;
//struct _type_env;
struct dts_type;

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

typedef struct _smacq_env {
  dts_environment * types;
} smacq_environment;

struct smacq_init;

typedef smacq_result smacq_thread_fn(struct smacq_init *);

struct smacq_init {
  int isfirst;
  int islast;
  char ** argv;
  int argc;
  smacq_environment * env;
  void * state;
  smacq_graph * self;
  smacq_thread_fn * thread_fn;
};


/* 
 * Comparisons & filtering
 */
enum _dts_comp_op { EXIST, EQ, LEQ, GEQ, NEQ, LT, GT, LIKE, AND, OR, FUNC };
typedef enum _dts_comp_op dts_compare_operation;

struct arglist;

typedef struct _dts_comparison {
  dts_compare_operation op;
  dts_field field;
  dts_object field_data;

  char * fieldname;
  char * valstr;
  struct arglist * arglist;
  int size;
  struct _dts_comparison * next;

  struct _dts_comparison * group;
} dts_comparison;

typedef struct _dts_msg {
  dts_object field_data;
  dts_comparison * criteria;
  struct _dts_msg * next;
} dts_message;

struct state; // Private to modules

typedef smacq_result smacq_produce_fn(struct state * state, const dts_object **, int * outchan);
typedef smacq_result smacq_consume_fn(struct state * state, const dts_object *, int * outchan);
typedef smacq_result smacq_init_fn(struct smacq_init *);
typedef smacq_result smacq_shutdown_fn(struct state *);

/*
 * Interrogation structures
 */
struct smacq_module_algebra {
  unsigned int vector:1;
  unsigned int boolean:1;
  unsigned int demux:1;
};

struct smacq_functions {
  smacq_produce_fn * produce;
  smacq_consume_fn * consume;
  smacq_init_fn * init;
  smacq_shutdown_fn * shutdown;
  smacq_thread_fn * thread;
  struct smacq_module_algebra algebra;
};

typedef int smacqtype_lt_fn(void *, int, void *, int);
typedef int smacqtype_fromstring_fn(char *, const dts_object *);


struct dts_type_info {
  int size;
  smacqtype_lt_fn * lt;
  smacqtype_fromstring_fn * fromstring;
};



/*
 * Utility routines that have no shared state 
 */
EXTERN int smacq_getoptsbyname(int argc, char ** argv, 
			      int * , char***, 
			      struct smacq_options *, 
			      struct smacq_optval *);

/*
 * Module threading routines 
 */

extern smacq_result smacq_thread_init(struct smacq_init * volatile_context);
extern smacq_result smacq_thread_consume(struct state * state, const dts_object * datum, int * outchan);
extern smacq_result smacq_thread_produce(struct state * state, const dts_object ** datum, int *outchan);
extern smacq_result smacq_thread_shutdown(struct state * state);

extern const dts_object * smacq_read(struct smacq_init * context);
extern void smacq_write(struct state * state, dts_object * datum, int outchan);
extern void smacq_decision(struct smacq_init * context, const dts_object * datum, smacq_result result);
extern void  smacq_flush(struct smacq_init * context);


/*
 * User Interface to main system
 */

enum smacq_scheduler { ITERATIVE, RECURSIVE, LOOP, THREADED };
EXTERN int smacq_start(smacq_graph *, enum smacq_scheduler, dts_environment *);
void smacq_init_modules(smacq_graph *, smacq_environment *);
EXTERN smacq_graph * smacq_build_pipeline(int argc, char ** argv);
smacq_graph * smacq_build_query(dts_environment * tenv, int argc, char ** argv);
int smacq_execute_query(int argc, char ** argv);
smacq_graph * smacq_add_new_child(smacq_graph * parent, int argc, char ** argv);
int smacq_add_child(smacq_graph * parent, smacq_graph * child);
void smacq_replace_child(smacq_graph * parent, int num, smacq_graph * newchild);
void smacq_add_parent(smacq_graph * child, smacq_graph * parent);
void smacq_remove_child(smacq_graph * parent, int childnum);
void smacq_remove_parent(smacq_graph * child, const smacq_graph * parent);
smacq_graph * smacq_merge_graphs(smacq_graph *);
smacq_graph * smacq_graph_add_graph(smacq_graph * a, smacq_graph * b);

smacq_graph * smacq_new_module(int argc, char ** argv);
EXTERN void smacq_free_module(smacq_graph * f);
EXTERN void smacq_destroy_graph(smacq_graph * f);
smacq_graph * smacq_clone_child(smacq_graph * parent, int child);
smacq_graph * smacq_clone_tree(smacq_graph * donorParent, smacq_graph * newParent, int child);

dts_comparison * dts_parse_tests(dts_environment * tenv, int argc, char ** argv);
char * dts_field_getname(dts_environment * tenv, dts_field f);
int smacq_graph_print(FILE * fh, smacq_graph * f, int indent);
void dts_free(const dts_object*);

#if defined(WIN32) && !defined(inline)       
	/* Visual C++ uses the keyword "__inline" rather than "inline" */
	#define inline __inline 
#endif

#include <smacq-internal.h>
#include <types-inline.c>

#endif

