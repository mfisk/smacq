#ifndef SMACQ_H
#define SMACQ_H

/*!

\version 2.1

\mainpage System for Modular Analysis and Continuous Queries

SMACQ is an extensible system for analyzing streams of structured
data.

\section smacq_embed Embedding SMACQ in Your Application

All work is done in one or more SmacqGraph instances.  The easiest way
to construct a SmacqGraph is with SmacqGraph::newQuery().  To execute a
graph, you also need to instantiate a SmacqScheduler and use its
methods like SmacqScheduler::input(), SmacqScheduler::busy_loop(),
etc.

\section dts_module Creating a SMACQ Type Module

Type modules define interfaces for parsing data.  See the dts-modules
manpage for more information.

\section smacq_module Creating a SMACQ Processing Module

A data-processing module should be a subclass of SmacqModule or
ThreadedSmacqModule.

\section using_dts Using a DtsObject

SMACQ uses the DtsObject abstraction for all data that it handles.
C++ programmers should ALWAYS use a DtsObject auto-pointer rather than
referencing the underlying DtsObject_ class directly.  Classes like
FieldVecHash and FieldVecSet are provided to make it easier to do
common tasks with a DtsObject.

*/

#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>
#include <gmodule.h>
#include <glib.h>

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

/// smacq_result is like an enum, except that you can OR and AND them
/// like flags. 
typedef flags<enum _smacq_result> smacq_result;

extern smacq_result SMACQ_NONE;
extern smacq_result SMACQ_FREE;
extern smacq_result SMACQ_PASS;
extern smacq_result SMACQ_ERROR;
extern smacq_result SMACQ_END;
extern smacq_result SMACQ_CANPRODUCE;
extern smacq_result SMACQ_PRODUCE;

/*
enum _smacq_result { SMACQ_NONE=0, SMACQ_FREE=1, SMACQ_PASS=2, SMACQ_ERROR=4, SMACQ_END=8, SMACQ_CANPRODUCE=256, SMACQ_PRODUCE=512};
typedef enum _smacq_result smacq_result;
*/
#endif

#include "util.c"
#include <dts-types.h>

class SmacqGraph;
class IterativeScheduler;

/// The only SmacqScheduler is currently IterativeSchedule, so it is a
/// typedef instead of a base class.
typedef IterativeScheduler SmacqScheduler;

enum smacq_log_level { INFO, WARN, ERROR, };

static inline void smacq_log(char * name, enum smacq_log_level level, char * msg) {
  fprintf(stderr, "%s: %s\n", name, msg);
}

BEGIN_C_DECLS

void * smacq_find_module(GModule ** gmodulep, char * envvar, char * envdefault, char * modformat, char * symformat, char * sym);

END_C_DECLS

#ifdef __cplusplus
#include <dts.h>
#endif

#endif

