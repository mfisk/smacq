#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <SmacqModule.h>
#include <FieldVec.h>
#include <SmacqGraph.h>
#include <SmacqScheduler.h>

#include <smacq_am.h>

static struct smacq_options options[] = {
  {"seed", {boolean_t:0}, "Seed remote graph to produce", SMACQ_OPT_TYPE_BOOLEAN},
  END_SMACQ_OPTIONS
};

SMACQ_MODULE(distribute,
  PROTO_CTOR(distribute);
  PROTO_DTOR(distribute);
  PROTO_CONSUME();

  int nodes;
  int ni;
); 

smacq_result distributeModule::consume(DtsObject datum, int & outchan) {
  // Send object to all other nodes
  Gasnet.RequestMedium(ni, AM_DTSOBJECT, datum->getdata(), datum->getsize(), datum->gettype());

  ni = ((ni + 1) % (nodes-1)) + 1; // Iterate over [1..nodes)
  return SMACQ_FREE;
}

distributeModule::~distributeModule() {
  for (int i = 0; i < nodes; ++i)
  	Gasnet.RequestShort(i, AM_ENDINPUT, 0);
}

distributeModule::distributeModule(struct SmacqModule::smacq_init * context) 
  : SmacqModule(context), ni(1)
{
  int argc;
  const char ** argv;

  smacq_opt seed;
  struct smacq_optval optvals[] = {
		{"seed", &seed},
    		{NULL, NULL}
  };
  smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

  // Distribute query to all other nodes
  for (int i = 1; i < nodes; ++i) {
  	Gasnet.RequestMedium(i, AM_QUERY, argv[0], strlen(argv[0])+1, seed.boolean_t, context->self);
  }

  nodes = gasnet_nodes();
}

