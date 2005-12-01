#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <smacq.h>
#include <SmacqGraph.h>
#include <SmacqScheduler.h>
#include "config.h"

#define MAX_QUERY_SIZE 4096*100

static struct smacq_options options[] = {
  {"t", {string_t:NULL}, "Describe the specified type", SMACQ_OPT_TYPE_STRING},
  {"m", {boolean_t:0}, "Multiple queries on STDIN", SMACQ_OPT_TYPE_BOOLEAN},
  {"f", {string_t:"-"}, "File to read queries from", SMACQ_OPT_TYPE_STRING},
  {"pregraph", {boolean_t:0}, "Show pre-initialization graph", SMACQ_OPT_TYPE_BOOLEAN},
  {"g", {boolean_t:0}, "Show final graph", SMACQ_OPT_TYPE_BOOLEAN},
  {"q", {boolean_t:0}, "Ignore warnings", SMACQ_OPT_TYPE_BOOLEAN},
  END_SMACQ_OPTIONS
};

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

int smacqq(int argc, char ** argv) {
  smacq_opt multiple, optimize, qfile, showpregraph, showgraph, showtype, quiet;
  int qargc;
  char ** qargv;
  DTS dts;
  FILE * fh;
  SmacqGraphContainer * graphs = NULL;
  DtsObject product;

#ifdef REFINFO
  signal(SIGUSR1, print_refs);
#endif

  if (argc <= 1) {
	  fprintf(stderr, "Usage: %s [-m] query\n", argv[0]);
	  fprintf(stderr, "Version %s; Build date %s\n", 
		  PACKAGE_VERSION, SMACQ_BUILD_DATE);
	  return -1;
  }

  struct smacq_optval optvals[] = {
		  {"t", &showtype},
		  {"m", &multiple},
		  {"f", &qfile},
		  {"O", &optimize},
		  {"pregraph", &showpregraph},
		  {"g", &showgraph},
		  {"q", &quiet},
		  {NULL, NULL}
  };
  smacq_getoptsbyname(argc-1, argv+1, &qargc, &qargv, options, optvals);

  if (quiet.boolean_t) {
	dts.set_no_warnings();
  }

  if (showtype.string_t) {
	dts_typeid tid = dts.requiretype(showtype.string_t);
   	assert(tid);
   	dts_type * t = dts.type_bynum(tid);
   	std::vector<struct dts_field_info*>::iterator i;

   	printf("Type \"%s\" defines the following fields:\n", showtype.string_t);
   	for (i=t->fields.begin(); i != t->fields.end(); ++i) {
		if (*i) {
       		printf("%30s: type %s\n", (*i)->desc.name, (*i)->desc.type);
		}
   	}
   	exit(0);
  }

  SmacqScheduler s;

  if (multiple.boolean_t) {
      char * queryline;
      int qno=1;

      queryline = (char*)malloc(MAX_QUERY_SIZE);

      if (qargc) {
	      fprintf(stderr, "Cannot specify query on command line with -m option\n");
	      return -1;
      }

      if (!strcmp(qfile.string_t, "-")) {
	      fh = stdin;
      } else {
	      fh = fopen(qfile.string_t, "r");
      }

      while(fgets(queryline, MAX_QUERY_SIZE, fh)) {
	      /* Chomp newline */
	      if (queryline[strlen(queryline)-1] == '\n')
		      queryline[strlen(queryline)-1] = '\0';

	      SmacqGraphContainer * newgraph = SmacqGraph::newQuery(&dts, &s, 1, &queryline);
	      if (!newgraph) {
		      fprintf(stderr, "Fatal error at line %d\n", qno);
		      exit(-1);
	      }
      	      if (graphs) {
		graphs->add_graph(newgraph, true);
	      } else {
		graphs = newgraph;
	      }
	      qno++;
      }

  } else {
    graphs = SmacqGraph::newQuery(&dts, &s, qargc, qargv);
    if (!graphs) exit(-1);
  }

  if (showpregraph.boolean_t) {
      graphs->print(stderr, 8);
  }

  graphs->init(&dts, &s);

  if (showgraph.boolean_t) {
      graphs->print(stderr, 8);
  }

  s.seed_produce(graphs);
  return (! s.busy_loop());
}

