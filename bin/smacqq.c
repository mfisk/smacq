#include <stdio.h>
#include <smacq.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <smacq.h>

#define MAX_QUERY_SIZE 4096*100

struct thread_args {
  smacq_graph * f;
  struct smacq_init * context;
};

static struct smacq_options options[] = {
  {"m", {boolean_t:0}, "Multiple queries on STDIN", SMACQ_OPT_TYPE_BOOLEAN},
  {"O", {boolean_t:0}, "Optimize multiple queries", SMACQ_OPT_TYPE_BOOLEAN},
  {"f", {string_t:"-"}, "File to read queries from", SMACQ_OPT_TYPE_STRING},
  {"g", {boolean_t:0}, "Show final graph", SMACQ_OPT_TYPE_BOOLEAN},
  {NULL, {string_t:NULL}, NULL, 0}
};


int main(int argc, char ** argv) {
  smacq_opt multiple, optimize, qfile, showgraph;
  int qargc;
  char ** qargv;
  dts_environment * tenv = dts_init();
  FILE * fh;
  smacq_graph * graphs = NULL;
  const dts_object * product;
  struct runq * runq = NULL;

  if (argc <= 1) {
	  fprintf(stderr, "Usage: %s [-m] query\n", argv[0]);
	  fprintf(stderr, "Build date %s\n", SMACQ_BUILD_DATE);
	  return -1;
  }

  {
	  struct smacq_optval optvals[] = {
		  {"m", &multiple},
		  {"f", &qfile},
		  {"O", &optimize},
		  {"g", &showgraph},
		  {NULL, NULL}
	  };
	  smacq_getoptsbyname(argc-1, argv+1, &qargc, &qargv, options, optvals);
  }

  if (multiple.boolean_t) {
      char * queryline;
      int qno=1;

      queryline = malloc(MAX_QUERY_SIZE);

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
	      smacq_graph * newgraph;

	      /* Chomp newline */
	      if (queryline[strlen(queryline)-1] == '\n')
		      queryline[strlen(queryline)-1] = '\0';

	      newgraph = smacq_build_query(tenv, 1, &queryline);
	      if (!newgraph) {
		      fprintf(stderr, "Fatal error at line %d\n", qno);
		      exit(-1);
	      }
      	      graphs = smacq_graph_add_graph(graphs, newgraph);
	      qno++;
      }

      if (optimize.boolean_t) {
	      graphs = smacq_merge_graphs(graphs);
      }
  } else {
      graphs = smacq_build_query(tenv, qargc, qargv);
      assert(graphs);
  }

  if (showgraph.boolean_t) {
      smacq_graphs_print(stderr, graphs, 8);
  }

  if (0 != smacq_start(graphs, ITERATIVE, tenv)) {
      return -1;
  }

  while(1) {
       int res = smacq_sched_iterative(graphs, NULL, &product, &runq, 1);
       if (product) {
	       //fprintf(stderr, "smacqq: Got selection!\n");
	       dts_decref(product);
       }
       if (res & SMACQ_END) {
	       return 0;
       }
  }
}

