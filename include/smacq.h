#ifndef SMACQ_H
#define SMACQ_H

#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>
#include <ccpp.h>

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

#include <pthread.h>
#ifdef SMACQ_OPT_NOPTHREADS
#define smacq_pthread_mutex_init(x,y)
#define smacq_pthread_mutex_destroy(x)
#define smacq_pthread_mutex_lock(x)
#define smacq_pthread_mutex_unlock(x)

#define smacq_pthread_cond_init(x,y)
#define smacq_pthread_cond_destroy(x)
#define smacq_pthread_cond_wait(x,y)
#define smacq_pthread_cond_broadcast(x)
#endif

#ifndef __cplusplus
#error "<smacq.h> can only be used in C++ programs"
#endif

#ifdef __cplusplus
/*
enum _smacq_result { SMACQ_NONE=0, SMACQ_FREE=1, SMACQ_PASS=2, SMACQ_ERROR=4, SMACQ_END=8, SMACQ_CANPRODUCE=256, SMACQ_PRODUCE=512};
typedef enum _smacq_result smacq_result;
*/
template<class N, typename T = int>
class flags {
	private:
		typedef flags this_type;
		T val;
	public:
		flags<N,T>() : val(0) {}
		flags<N,T>(const T x) : val(x) {}

		flags<N,T> 	operator |  (const flags<N,T> &x) 	const { return val | x.val; }
		flags<N,T> 	operator &  (const flags<N,T> &x) 	const { return val & x.val; }
	 	flags<N,T> 	operator |= (const flags<N,T> &x) 	      { return val |= x.val; }
		//int  		operator () () 				const { return val; }
		bool  		operator !  () 				const { return val == 0; }
		bool  		operator == (const flags<N,T> &x) 	const { return val == x.val; }
		bool  		operator != (const flags<N,T> &x) 	const { return val != x.val; }

		// This is crazy:
		typedef T this_type::*unspecified_bool_type;
		operator unspecified_bool_type () const { return (val == 0? 0: &this_type::val); }

};

enum _smacq_result {};
typedef flags<enum _smacq_result> smacq_result;

extern smacq_result SMACQ_NONE;
extern smacq_result SMACQ_FREE;
extern smacq_result SMACQ_PASS;
extern smacq_result SMACQ_ERROR;
extern smacq_result SMACQ_END;
extern smacq_result SMACQ_CANPRODUCE;
extern smacq_result SMACQ_PRODUCE;
#endif

#define SMACQ_MULTITHREAD 65536

struct smacq_init;
struct arglist;
struct state; // Private to modules

#include "util.c"
#include "darray.c"
#include "smacq_args.h"

#include <dts-types.h>
class SmacqGraph;
class IterativeScheduler;
typedef IterativeScheduler SmacqScheduler;

struct smacq_init {
  SmacqScheduler * scheduler;
  bool isfirst;
  bool islast;
  char ** argv;
  int argc;
  DTS * dts;
  void * state;
  SmacqGraph * self;
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

//typedef smacq_result smacq_produce_fn(struct state * state, DtsObject, int & outchan);
//typedef smacq_result smacq_consume_fn(struct state * state, DtsObject, int & outchan);
//typedef smacq_result smacq_init_fn(struct smacq_init *);
//typedef smacq_result smacq_shutdown_fn(struct state *);
#include <SmacqModule.h>
typedef SmacqModule*  smacq_constructor_fn(struct smacq_init *);

struct smacq_functions {
  smacq_constructor_fn * constructor;
  struct smacq_module_algebra algebra;
/* Put constructor and algebra first so that we can use partial initializers in g++ */
};

BEGIN_C_DECLS

SmacqGraph * smacq_build_query(DTS * tenv, int argc, char ** argv);

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

#define PROTO_CONSUME() public: smacq_result consume(DtsObject, int&); private:
#define PROTO_PRODUCE() public: smacq_result produce(DtsObject&, int&); private:
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

