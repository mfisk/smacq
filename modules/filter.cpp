#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <smacq.h>
#include <string.h>

#define SMACQ_MODULE_IS_STATELESS 1

SMACQ_MODULE(filter,
  PROTO_CTOR(filter);
  PROTO_CONSUME();
  
		     private:
		     char ** argv;
		     int argc;
		     dts_comparison * comp;
		     int type;
		     );

static struct smacq_options options[] = {
  END_SMACQ_OPTIONS
};

smacq_result filterModule::consume(DtsObject datum, int * outchan) {
  assert(datum);
  assert(argv[0]);
  
  type = datum->gettype();

  if (datum->match(comp)) 
    return SMACQ_PASS;
  else
    return SMACQ_FREE;
}

filterModule::filterModule(struct smacq_init * context) : SmacqModule(context) {
  {
    struct smacq_optval optvals[] = {
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &argc, &argv,
				 options, optvals);
    assert(argv);
  }

  comp = dts->parse_tests(argc, argv);
  if (!comp) assert(0);
}

