#include <stdio.h>
#include <glib.h>
#include <gmodule.h>
#include <smacq.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <smacq-internal.h>

#define MAX_QUERY_SIZE 4096

struct thread_args {
  smacq_graph * f;
  struct smacq_init * context;
};

static struct smacq_options options[] = {
  {"m", {boolean_t:0}, "Multiple queries on STDIN", SMACQ_OPT_TYPE_BOOLEAN},
  {NULL, {string_t:NULL}, NULL, 0}
};


int main(int argc, char ** argv) {
  smacq_graph * graph;
  smacq_opt multiple;
  int qargc;
  char ** qargv;

  if (argc <= 1) {
	  fprintf(stderr, "Usage: %s [-m] query\n", argv[0]);
	  return -1;
  }

  {
	  struct smacq_optval optvals[] = {
		  {"m", &multiple},
		  {NULL, NULL}
	  };
	  smacq_getoptsbyname(argc-1, argv+1, &qargc, &qargv, options, optvals);
  }


  if (multiple.boolean_t) {
      char * queryline;
      smacq_graph * graphs = NULL;
      const dts_object * product;
      void * runq = NULL;

      queryline = malloc(MAX_QUERY_SIZE);

      while(fgets(queryline, MAX_QUERY_SIZE, stdin)) {
	      smacq_graph * newgraph = smacq_build_query(1, &queryline);
	      assert(newgraph);
      	      graphs = smacq_merge_graphs(graphs, newgraph);
      }

      if (0 != smacq_start(graphs, ITERATIVE, NULL)) {
	      return -1;
      }

      while(1) {
	   smacq_graph * g;
	   for (g = graphs; g; g=g->next_graph) {
	       smacq_sched_iterative(g, NULL, &product, &runq, 1);
	   }
      }
      
  } else {
      graph = smacq_build_query(qargc, qargv);
      assert(graph);
      return smacq_start(graph, RECURSIVE, NULL);
  }
}

