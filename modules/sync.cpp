#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
#include <FieldVec.h>

static struct smacq_options options[] = {
  END_SMACQ_OPTIONS
};

SMACQ_MODULE(sync,
  PROTO_CTOR(sync);
  PROTO_CONSUME();
  PROTO_PRODUCE();

  DtsObject product;
  struct runq * runq;
  SmacqGraph * graph;
  smacq_result done; 
  int isfirst;
); 

smacq_result syncModule::consume(DtsObject datum, int & outchan) {
 smacq_result more = smacq_sched_iterative(graph, datum, &product, &runq, isfirst);

 more = (more ? 0 : SMACQ_END);

  if (datum && (product == datum)) {
	  return (smacq_result)(SMACQ_PASS|more);
  } else if (product) { 
	  done = more;
	  return (smacq_result)(SMACQ_FREE|SMACQ_PRODUCE);
  } else {
	  return (smacq_result)(SMACQ_FREE|more);
  }
  
  return SMACQ_PASS;
}

syncModule::syncModule(struct SmacqModule::smacq_init * context) : SmacqModule(context) {
  int argc = context->argc-1;
  char ** argv = context->argv+1;
  isfirst = context->isfirst;
  int i;

  for (i=0; i<argc; i++) {
	  if (!strcmp(argv[i], "\\|")) {
		  argv[i] = "|";
	  }
  }

  graph = SmacqGraph::newQuery(dts, argc, argv);
  graph->init(dts);
  sched = new IterativeScheduler(graph, false);
  sched->busy()
}

smacq_result syncModule::produce(DtsObject & datump, int & outchan) {
  if (isfirst && !product) {
	consume(NULL, outchan);
  }
  if (product) {
	datump = product;
	product = NULL;
	if (isfirst) {
		return (smacq_result) (SMACQ_PASS| (done ? SMACQ_END : SMACQ_PRODUCE));
	} else {
		return (smacq_result) (SMACQ_PASS| (done ? SMACQ_END : 0));
	}
  } else {
	return SMACQ_FREE;
  }
}

