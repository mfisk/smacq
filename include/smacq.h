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
# define SMACQ_OPT_RUNRING
# define SMACQ_OPT_DTS_FREELIST
#endif

#ifdef SMACQ_NO_OPT_DTS
# define SMACQ_OPT_NOFIELDCACHE
#endif

#ifdef SMACQ_NO_OPT_DATAFLOW
# define SMACQ_OPT_NOVECTORS
# define SMACQ_OPT_NOCHILDREN
# define SMACQ_OPT_NOHEADS
# define SMACQ_OPT_NOTAILS
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

class SmacqGraph;
class IterativeScheduler;

/// The only SmacqScheduler is currently IterativeSchedule, so it is a
/// typedef instead of a base class.
typedef IterativeScheduler SmacqScheduler;

enum smacq_log_level { INFO, WARN, ERROR, };

static inline void smacq_log(char * name, enum smacq_log_level level, char * msg) {
  fprintf(stderr, "%s: %s\n", name, msg);
}

#include <smacq_result.h>
#include <dts.h>
#include <dts-types.h>

BEGIN_C_DECLS

void * smacq_find_module(GModule ** gmodulep, char * envvar, char * envdefault, char * modformat, char * symformat, char * sym);

#include "util.c"

static inline char * argv2str(int argc, char ** argv) {
  char * qstr;
  int i;
  int size = 1;

  for (i=0; i<argc; i++) {
    size += strlen(argv[i]);
  }
  size += argc;
  
  qstr = (char*)malloc(size);
  qstr[0] = '\0';
    
  for (i=0; i<argc; i++) {
    strcatn(qstr, size, argv[i]);
    strcatn(qstr, size, " ");
  }
  return qstr;
}


END_C_DECLS

#endif

