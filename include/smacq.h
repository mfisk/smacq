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

#ifdef __cplusplus
class DTS;
class DtsObject;
#else
typedef void DTS;
typedef void DtsObject;
#endif

struct smacq_init {
  int isfirst;
  int islast;
  char ** argv;
  int argc;
  DTS * dts;
  void * state;
  smacq_graph * self;
};

/*
 * Interrogation structures
 */
struct smacq_module_algebra {
  unsigned int stateless:1;
  unsigned int vector:1;
  unsigned int annotation:1;
  unsigned int demux:1;
};

typedef smacq_result smacq_produce_fn(struct state * state, DtsObject **, int * outchan);
typedef smacq_result smacq_consume_fn(struct state * state, DtsObject *, int * outchan);
typedef smacq_result smacq_init_fn(struct smacq_init *);
typedef smacq_result smacq_shutdown_fn(struct state *);
#include <SmacqModule.h>
typedef SmacqModule*  smacq_constructor_fn(struct smacq_init *);

struct smacq_functions {
  smacq_constructor_fn * constructor;
  struct smacq_module_algebra algebra;
/* Put constructor and algebra first so that we can use partial initializers in g++ */

  smacq_produce_fn * produce;
  smacq_consume_fn * consume;
  smacq_init_fn * init;
  smacq_shutdown_fn * shutdown;
};

BEGIN_C_DECLS

/*
 * User Interface to main system
 */
enum smacq_scheduler { ITERATIVE, RECURSIVE, LOOP, THREADED };
int smacq_start(smacq_graph *, enum smacq_scheduler, DTS *);
void smacq_init_modules(smacq_graph *, DTS *);
smacq_graph * smacq_build_pipeline(int argc, char ** argv);
smacq_graph * smacq_build_query(DTS * tenv, int argc, char ** argv);
int smacq_execute_query(int argc, char ** argv);
smacq_graph * smacq_add_new_child(smacq_graph * parent, int argc, char ** argv);
int smacq_add_child(smacq_graph * parent, smacq_graph * child);
void smacq_replace_child(smacq_graph * parent, int num, smacq_graph * newchild);
void smacq_add_parent(smacq_graph * child, smacq_graph * parent);
void smacq_remove_child(smacq_graph * parent, int childnum);
void smacq_remove_parent(smacq_graph * child, const smacq_graph * parent);
smacq_graph * smacq_merge_graphs(smacq_graph *);
smacq_graph * smacq_graph_add_graph(smacq_graph * a, smacq_graph * b);
smacq_graph * smacq_graph_clone(DTS *, smacq_graph *);

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

#ifdef __cplusplus

/* Usage:

SMACQ_MODULE(foo,
  private:
    ...
);

*/

#define PROTO_CONSUME() public: smacq_result consume(DtsObject *, int*); private:
#define PROTO_PRODUCE() public: smacq_result produce(DtsObject **, int*); private:
#define PROTO_CTOR(name) public: name##Module::name##Module(smacq_init *); private:
#define PROTO_DTOR(name) public: name##Module::~name##Module(); private:

#define SMACQ_MODULE(name,defs) \
 class name##Module : public SmacqModule {	\
  private:					\
      defs					\
  };					        \
  EXPORT_SMACQ_MODULE(name, MODULE_ALGEBRA)

#define SMACQ_MODULE_THREAD(name,defs) \
 class name##Module : public ThreadedSmacqModule {	\
  public:					\
    smacq_result thread(smacq_init* context);	\
    name##Module(smacq_init * context) : ThreadedSmacqModule(context) {} \
  private:					\
      defs					\
  };					        \
  EXPORT_SMACQ_MODULE(name, MODULE_ALGEBRA)

#define MODULE_ALGEBRA { \
	stateless: SMACQ_MODULE_IS_STATELESS, \
	vector: SMACQ_MODULE_IS_VECTOR, \
 	annotation: SMACQ_MODULE_IS_ANNOTATION, \
	demux: SMACQ_MODULE_IS_DEMUX }

#define SMACQ_MODULE_IS_STATELESS 0
#define SMACQ_MODULE_IS_VECTOR 0
#define SMACQ_MODULE_IS_ANNOTATION 0
#define SMACQ_MODULE_IS_DEMUX 0

#define EXPORT_SMACQ_MODULE(name, alg)					\
  SMACQ_MODULE_CONSTRUCTOR(name);					\
  struct smacq_functions smacq_##name##_table = {			\
    constructor: &name##_constructor,					\
    algebra: alg							\
  };

#define SMACQ_MODULE_CONSTRUCTOR(name) \
  static SmacqModule * name##_constructor(struct smacq_init * context) { \
    return new name##Module(context);					\
  }				

#include <dts.h>
#endif

#endif

