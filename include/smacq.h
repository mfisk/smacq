#ifndef SMACQ_H
#define SMACQ_H

#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>

/* Turn on performance optimizations: */
#ifdef SMACQ_NO_OPT
# define SMACQ_NO_OPT_DTS
# define SMACQ_NO_OPT_DATAFLOW
#endif

#ifndef SMACQ_NO_OPT_DTS
# define SMACQ_OPT_NOPTHREADS
# define SMACQ_OPT_NOMSGS
# define SMACQ_OPT_FORCEFIELDCACHE
# define SMACQ_OPT_RUNRING
# define SMACQ_OPT_DTS_FREELIST
#endif

#ifndef SMACQ_NO_OPT_DATAFLOW
# define SMACQ_OPT_VECTORS
# define SMACQ_OPT_CHILDREN
# define SMACQ_OPT_HEADS
# define SMACQ_OPT_TAILS
#endif

#if defined(WIN32) && !defined(inline)       
	/* Visual C++ uses the keyword "__inline" rather than "inline" */
	#define inline __inline 
#endif

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

#ifndef BEGIN_C_DECLS
# ifdef __cplusplus
#   define BEGIN_C_DECLS extern "C" {
#   define END_C_DECLS }
# else
#   define BEGIN_C_DECLS
#   define END_C_DECLS
# endif
#endif

enum _smacq_result { SMACQ_FREE=1, SMACQ_PASS=2, SMACQ_ERROR=4, SMACQ_END=8, SMACQ_CANPRODUCE=256, SMACQ_PRODUCE=512};
typedef enum _smacq_result smacq_result;

#define SMACQ_MULTITHREAD 65536

struct smacq_init;
struct arglist;
struct state; // Private to modules
struct _smacq_module;
typedef struct _smacq_module smacq_graph;

#include "util.c"
#include "darray.c"
#include "smacq_args.h"
#include "dts.h"

typedef struct _smacq_env {
  dts_environment * types;
} smacq_environment;

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
 * Interrogation structures
 */
struct smacq_module_algebra {
  unsigned int vector:1;
  unsigned int boolean:1;
  unsigned int demux:1;
  unsigned int nesting:1;
};

typedef smacq_result smacq_produce_fn(struct state * state, const dts_object **, int * outchan);
typedef smacq_result smacq_consume_fn(struct state * state, const dts_object *, int * outchan);
typedef smacq_result smacq_init_fn(struct smacq_init *);
typedef smacq_result smacq_shutdown_fn(struct state *);
#include <SmacqModule.h>
typedef SmacqModule*  smacq_constructor_fn(struct smacq_init *);

struct smacq_functions {
  smacq_produce_fn * produce;
  smacq_consume_fn * consume;
  smacq_init_fn * init;
  smacq_shutdown_fn * shutdown;
  smacq_thread_fn * thread;
  smacq_constructor_fn * constructor;
  struct smacq_module_algebra algebra;
};

BEGIN_C_DECLS

/*
 * Module threading routines 
 */
smacq_result smacq_thread_init(struct smacq_init * volatile_context);
smacq_result smacq_thread_consume(struct state * state, const dts_object * datum, int * outchan);
smacq_result smacq_thread_produce(struct state * state, const dts_object ** datum, int *outchan);
smacq_result smacq_thread_shutdown(struct state * state);

#define SMACQ_THREADED_MODULE(THREAD) { \
	init: smacq_thread_init, \
	consume: smacq_thread_consume, \
	produce: smacq_thread_produce, \
	shutdown: smacq_thread_shutdown, \
	thread: THREAD };

const dts_object * smacq_read(struct smacq_init * context);
void smacq_write(struct state * state, dts_object * datum, int outchan);
void smacq_decision(struct smacq_init * context, const dts_object * datum, smacq_result result);
int smacq_flush(struct smacq_init * context);


/*
 * User Interface to main system
 */
enum smacq_scheduler { ITERATIVE, RECURSIVE, LOOP, THREADED };
int smacq_start(smacq_graph *, enum smacq_scheduler, dts_environment *);
void smacq_init_modules(smacq_graph *, smacq_environment *);
smacq_graph * smacq_build_pipeline(int argc, char ** argv);
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
smacq_graph * smacq_graph_clone(smacq_environment *, smacq_graph *);

smacq_graph * smacq_new_module(int argc, char ** argv);
void smacq_free_module(smacq_graph * f);
void smacq_destroy_graph(smacq_graph * f);
smacq_graph * smacq_clone_child(smacq_graph * parent, int child);
smacq_graph * smacq_clone_tree(smacq_graph * donorParent, smacq_graph * newParent, int child);

int smacq_graph_print(FILE * fh, smacq_graph * f, int indent);
int smacq_graphs_print(FILE * fh, smacq_graph * f, int indent);
double smacq_graph_count_nodes(smacq_graph * f);

END_C_DECLS

enum smacq_log_level { INFO, WARN, ERROR, };

static inline void smacq_log(char * name, enum smacq_log_level level, char * msg) {
  fprintf(stderr, "%s: %s\n", name, msg);
}

#include "smacq-internal.h"
#include "types-inline.c"


#endif

