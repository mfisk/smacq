#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define SMACQ_MODULE_IS_STATELESS 1
#include <smacq.h>
#include <SmacqGraph.h>

SMACQ_MODULE(lor,
  PROTO_CTOR(lor);

  SmacqGraph * graphs;
  void build_clause(char ** argv, int num);
);

void lorModule::build_clause(char ** argv, int num) {
  /* End of query */
  SmacqGraph * g;

  //fprintf(stderr, "Build clause from %s(%p)+%d ... %s\n", argv[0], argv, num, argv[num-1]);

  if (num < 1) {
    fprintf(stderr, "OR: empty clause\n");
    return;
  }

  g = SmacqGraph::newQuery(dts, num, argv);
  assert(g);

  if (graphs) {
    graphs->add_graph(g);
  } else {
    graphs = g;
  }
}

lorModule::lorModule(struct SmacqModule::smacq_init * context)
  : SmacqModule(context), graphs(NULL)
{
  int argc = context->argc-1;
  char ** argv = context->argv+1;

  {
    int start = 0;
    int i;
    for (i=0; i<argc; i++) {
      if (!strcmp(argv[i], ";")) {
	build_clause(argv+start, i - start);
	start = i+1;
      }
    }
    if (start < argc) {
      build_clause(argv+start, argc - start);
    }
  }

  graphs->init(dts, context->scheduler);
  context->self->replace(graphs);
}
