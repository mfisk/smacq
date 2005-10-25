#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <SmacqModule.h>
#include <FieldVec.h>
#include <SmacqGraph.h>
#include <SmacqScheduler.h>

static struct smacq_options options[] = {
  {"seed", {boolean_t:0}, "Seed remote graph to produce", SMACQ_OPT_TYPE_BOOLEAN},
  END_SMACQ_OPTIONS
};

SMACQ_MODULE(distribute,
  PROTO_CTOR(distribute);
  PROTO_DTOR(distribute);
  PROTO_CONSUME();
  PROTO_PRODUCE();
); 

smacq_result groupbyModule::consume(DtsObject datum, int & outchan) {
  

  return SMACQ_FREE;
}

groupbyModule::~groupbyModule() {
}

groupbyModule::groupbyModule(struct SmacqModule::smacq_init * context) 
  : SmacqModule(context), self(context->self)
{
  int argc;
  char ** argv;

  smacq_opt seed;
  struct smacq_optval optvals[] = {
		{"seed", &ptr},
    		{NULL, NULL}
  };
  smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

  char ** exec_argv = (char**)malloc(sizeof(char*)*4);
  exec_argv[0] = "smacqq";
  exec_argv[1] = "slave";
  exec_argv[2] = argv[0];

  if (fork) {
	// parent
	/// XXX. Handle AM callbacks
  } else {
	// child
	execvp(exec_argv[0], exec_argv);
	exit(-1); // Shouldn't get here
  }
}


