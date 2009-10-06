#ifndef SMACQ_H
#define SMACQ_H

/*!

\version 2.7

\mainpage System for Modular Analysis and Continuous Queries

SMACQ is an extensible system for analyzing streams of structured
data.

\section smacq_embed Embedding SMACQ in Your Application

You will need to instantiate a DTS type system object, a SmacqScheduler, and 
a SmacqGraph.  Then use SmacqGraph::addQuery() to parse one or more queries. 
Finally use one of the SmacqScheduler methods like 
SmacqScheduler::input() or SmacqScheduler::busy_loop().

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
#include <ltdl.h>
#include <glib.h>

#include <ccpp.h>

/* Turn on performance optimizations: */
#ifdef SMACQ_NO_OPT
# define SMACQ_NO_OPT_DTS
# define SMACQ_NO_OPT_DATAFLOW
#endif

#ifndef SMACQ_NO_OPT_DTS
# define SMACQ_OPT_NOMSGS
# define SMACQ_OPT_RUNRING
# define SMACQ_OPT_DTS_FREELIST
#endif

#ifdef SMACQ_NO_OPT_DTS
# define SMACQ_OPT_NOFIELDCACHE
#endif

#if defined(WIN32) && !defined(inline)       
	/* Visual C++ uses the keyword "__inline" rather than "inline" */
	#define inline __inline 
#endif

#ifdef __cplusplus
class SmacqGraphNode;
class SmacqScheduler;

#include <boost/intrusive_ptr.hpp>
/// A reference-counted pointer to a SmacqGraphNode.
typedef boost::intrusive_ptr<SmacqGraphNode> SmacqGraphNode_ptr;

enum smacq_log_level { INFO, WARN, ERROR, };

static inline void smacq_log(const char * name, enum smacq_log_level level, const char * msg) {
  fprintf(stderr, "%s: %s\n", name, msg);
}

#include <smacq_result.h>
#include <dts.h>
#include <dts-types.h>
#include <util.c>
#endif //__cplusplus

BEGIN_C_DECLS
/// Parse a query and run to completion
int smacqq(int argc, const char ** argv);

void * smacq_find_module(lt_dlhandle* gmodulep, const char * envvar, const char * envdefault, const char * modformat, const char * symformat, const char * sym);
END_C_DECLS

#endif

