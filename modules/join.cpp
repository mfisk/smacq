#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
#include <FieldVec.h>

static struct smacq_options options[] = {
  END_SMACQ_OPTIONS
};

struct join {
  SmacqGraph * graph;
  dts_field field;
  struct runq * runq;
};

SMACQ_MODULE(join,
  PROTO_CTOR(join);
  PROTO_CONSUME();
  PROTO_PRODUCE();

  int numjoins;
  int whereargc;
  char ** whereargv;
  DtsObject product;
  dts_comparison * comp;

  struct join * joins;
  static void find_join(dts_comparison * comp);
); 

smacq_result joinModule::consume(DtsObject datum, int & outchan) {
  smacq_result more;
  int i;

  for (i=0; i<numjoins; i++) {
	struct join * j = &joins[i];
  	DtsObject product;

  	more = smacq_sched_iterative(j->graph, NULL, &product, &j->runq, 1);
	datum->attach_field(j->field, product);
	//fprintf(stderr, "attaching %p to %p field %d\n", product, datum, j->field);
  }
  
  return SMACQ_PASS;
}

void joinModule::find_join(dts_comparison * comp) {
	if (!comp) return;

	/* Look for equality between fields in different joins */

}

joinModule::joinModule(struct SmacqModule::smacq_init * context) : SmacqModule(context) {
  int i, j;
  int argc = context->argc-1;
  char ** argv = context->argv+1;

  whereargc = 0;
  for (i=0; i<argc; i++) {
    if (!strcmp(argv[i], "--")) {
      whereargc = argc - i - 1;
      whereargv = argv+(i+1);
      break;
    }
  }

  argc -= (whereargc);

  // comp = dts_parse_tests(dts, whereargc, whereargv);
  
  // Consume rest of arguments as joins
  assert(argc > 0);
  assert((argc % 2) == 0);

  joins = (join*)calloc(argc, sizeof(struct join));
  numjoins = argc/2; 

  for (i=0, j=0; i<argc; i+=2, j++) {
	  //char fstr[256];
	  //snprintf(fstr, 256, "j%d", argv[i]);
	  joins[j].field = dts->requirefield(argv[i+1]);
	  joins[j].graph = SmacqGraph::newQuery(dts, 1, &argv[i]);
	  assert(joins[j].graph);
	  smacq_start(joins[j].graph, ITERATIVE, dts);
  }

  find_join(comp);
}

smacq_result joinModule::produce(DtsObject & datump, int & outchan) {
  smacq_result status;

  if (product) {
    datump = product;
    status = SMACQ_PASS;
  } else {
    status = SMACQ_FREE;
  }

  return (smacq_result)(status | (product ? SMACQ_PRODUCE : 0));
}

