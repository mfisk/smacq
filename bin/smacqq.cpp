#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <smacq.h>
#include <SmacqGraph.h>
#include <SmacqScheduler.h>
#include "config.h"

#define MAX_QUERY_SIZE 4096*100

struct thread_args {
  SmacqGraph * f;
  struct SmacqModule::smacq_init * context;
};

static struct smacq_options options[] = {
  {"t", {string_t:NULL}, "Describe the specified type", SMACQ_OPT_TYPE_STRING},
  {"m", {boolean_t:0}, "Multiple queries on STDIN", SMACQ_OPT_TYPE_BOOLEAN},
  {"O", {boolean_t:0}, "Optimize multiple queries", SMACQ_OPT_TYPE_BOOLEAN},
  {"f", {string_t:"-"}, "File to read queries from", SMACQ_OPT_TYPE_STRING},
  {"g", {boolean_t:0}, "Show final graph", SMACQ_OPT_TYPE_BOOLEAN},
  END_SMACQ_OPTIONS
};


int main(int argc, char ** argv) {
  smacq_opt multiple, optimize, qfile, showgraph, showtype;
  int qargc;
  char ** qargv;
  DTS dts;
  FILE * fh;
  SmacqGraph * graphs = NULL;
  DtsObject product;

  if (argc <= 1) {
	  fprintf(stderr, "Usage: %s [-m] query\n", argv[0]);
	  fprintf(stderr, "Version %s; Build date %s\n", 
		  PACKAGE_VERSION, SMACQ_BUILD_DATE);
	  return -1;
  }

  {
	  struct smacq_optval optvals[] = {
		  {"t", &showtype},
		  {"m", &multiple},
		  {"f", &qfile},
		  {"O", &optimize},
		  {"g", &showgraph},
		  {NULL, NULL}
	  };
	  smacq_getoptsbyname(argc-1, argv+1, &qargc, &qargv, options, optvals);
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
	      SmacqGraph * newgraph;

	      /* Chomp newline */
	      if (queryline[strlen(queryline)-1] == '\n')
		      queryline[strlen(queryline)-1] = '\0';

	      newgraph = SmacqGraph::newQuery(&dts, 1, &queryline);
	      if (!newgraph) {
		      fprintf(stderr, "Fatal error at line %d\n", qno);
		      exit(-1);
	      }
      	      if (graphs) {
		graphs->add_graph(newgraph);
	      } else {
		graphs = newgraph;
	      }
	      qno++;
      }

  } else {
      graphs = SmacqGraph::newQuery(&dts, qargc, qargv);
      assert(graphs);
  }

  if (optimize.boolean_t) {
    graphs->optimize();
  }

  SmacqScheduler * s = new SmacqScheduler(&dts, graphs, true);

  if (showgraph.boolean_t) {
      graphs->print(stderr, 8);
  }

  return (! s->busy_loop());
}

