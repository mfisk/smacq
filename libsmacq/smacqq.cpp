#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <smacq.h>
#include <SmacqGraph.h>
#include <SmacqScheduler.h>
#include "config.h"
#include <ThreadSafe.h>
#include <iostream>
#include <fstream>
#include <exception>

#define MAX_QUERY_SIZE 4096*100

static struct smacq_options options[] = {
  {"c", {int_t:1}, "Number of CPUs (threads) to use [EXPERIMENTAL]", SMACQ_OPT_TYPE_INT},
  {"t", {string_t:NULL}, "Describe the specified type", SMACQ_OPT_TYPE_STRING},
  {"f", {string_t:NULL}, "Read queries from file (- for STDIN)", SMACQ_OPT_TYPE_STRING},
  {"pregraph", {boolean_t:0}, "Show pre-initialization graph", SMACQ_OPT_TYPE_BOOLEAN},
  {"g", {boolean_t:0}, "Show final graph", SMACQ_OPT_TYPE_BOOLEAN},
  {"q", {boolean_t:0}, "Ignore warnings", SMACQ_OPT_TYPE_BOOLEAN},
  {"debug", {boolean_t:0}, "Add verbose diagnostics", SMACQ_OPT_TYPE_BOOLEAN},
  END_SMACQ_OPTIONS
};

char * program_name;

#ifdef REFINFO

#include <signal.h>
RefMap REFS;

inline void print_refs(int x) {
        FILE * fh = fopen("/tmp/smacq.refs", "w");
        std::map<void*,std::map<void*, Counter> >::iterator i;
        std::map<void*, Counter>::iterator j;
        for (i=REFS.begin(); i != REFS.end(); ++i) {
           for (j=i->second.begin(); j != i->second.end(); ++j) {
                if (j->second.count) {
                   fprintf(fh, "%p\t%p\t%hd\n", i->first, j->first, j->second.count);
                }
           }
        }
        fclose(fh);
}
#endif

void print_field(dts_field_info * i) {
	if (i) printf("%30s: type %s\n", i->desc.name, i->desc.type);
}

int smacqq(int argc, char ** argv) {
 try {
  smacq_opt optimize, qfile, showpregraph, showgraph, cpus, showtype, quiet, debug;
  int qargc;
  char ** qargv;
  DTS dts;
  FILE * fh;
  DtsObject product;
  program_name = argv[0]; // For libgnu.

#ifdef REFINFO
  signal(SIGUSR1, print_refs);
#endif

  if (argc <= 1) {
	  fprintf(stderr, "Usage: %s [-m] query\n", argv[0]);
	  fprintf(stderr, "Version %s; Build date %s\n", 
		  PACKAGE_VERSION, SMACQ_BUILD_DATE);
	  return -1;
  }

  program_name = argv[0];

  struct smacq_optval optvals[] = {
		  {"c", &cpus},
		  {"t", &showtype},
		  {"f", &qfile},
		  {"O", &optimize},
		  {"pregraph", &showpregraph},
		  {"g", &showgraph},
		  {"q", &quiet},
		  {"debug", &debug},
		  {NULL, NULL}
  };
  smacq_getoptsbyname(argc-1, argv+1, &qargc, &qargv, options, optvals);

  if (quiet.boolean_t) {
	dts.set_no_warnings();
  }
  
  if (showtype.string_t) {
	dts_typeid tid = dts.requiretype(showtype.string_t);
   	assert(tid);
   	printf("Type \"%s\" defines the following fields:\n", showtype.string_t);
   	dts_type * t = dts.type_bynum(tid);
	t->fields.foreach(&print_field);
	return(0);
  }

  SmacqScheduler s;
  SmacqGraph graphs;

  if (debug.boolean_t) {
	s.setDebug();
  }

  if (qfile.string_t) {
      char * queryline;

      queryline = (char*)malloc(MAX_QUERY_SIZE);

      if (qargc) {
	      fprintf(stderr, "Cannot specify query on command line with -f option\n");
	      return -1;
      }

      if (!strcmp(qfile.string_t, "-")) {
	      fh = popen("cpp", "r");
      } else {
	      std::string s("cpp ");
	      s += qfile.string_t;
	      fh = popen(s.c_str(), "r");
      }
      while(fgets(queryline, MAX_QUERY_SIZE, fh)) {
	      graphs.addQuery(&dts, &s, queryline);
      }

  } else {
    graphs.addQuery(&dts, &s, argv2string(qargc, qargv));
  }

  if (graphs.empty()) {
	throw "Nothing to do (no query given)";
  }

  if (showpregraph.boolean_t) {
      graphs.print(stderr, 8);
  }

  graphs.init(&dts, &s);

  if (showgraph.boolean_t) {
      graphs.print(stderr, 8);
  }

  s.seed_produce(&graphs);

  s.start_threads(cpus.int_t - 1);

  // Work yourself too
  bool retval = (! s.busy_loop());

  return retval;
 } catch (std::exception& e) {
	std::cerr << e.what() << std::endl;
        return -1;
 }
}

