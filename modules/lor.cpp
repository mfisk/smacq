#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define SMACQ_MODULE_IS_STATELESS 1
#include <SmacqModule.h>
#include <SmacqGraph.h>

SMACQ_MODULE(lor,
	     PROTO_CTOR(lor);
	     
	     SmacqGraph graphs;
	     SmacqScheduler * sched;
	     void build_clause(const char ** argv, int num);
);

void lorModule::build_clause(const char ** argv, int num) {
  //fprintf(stderr, "Build clause from %s(%p)+%d ... %s\n", argv[0], argv, num, argv[num-1]);

  if (num < 1) {
    fprintf(stderr, "OR: empty clause\n");
    return;
  }

  graphs.addQuery(dts, sched, argv2string(num, argv));
}

lorModule::lorModule(struct SmacqModule::smacq_init * context)
  : SmacqModule(context), sched(context->scheduler)
{
  int argc = context->argc-1;
  const char ** argv = context->argv+1;

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

  graphs.init(dts, context->scheduler);
  context->self->replace(&graphs);
}
