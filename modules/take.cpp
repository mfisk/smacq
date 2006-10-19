#include <stdlib.h>
#include <assert.h>
#include <SmacqModule.h>

SMACQ_MODULE(take,
  PROTO_CTOR(take);
  PROTO_CONSUME();

  DtsField field;
); 
 
smacq_result takeModule::consume(DtsObject datum, int & outchan) {
  DtsObject inside = datum->getfield(field);
  enqueue(inside);
  return SMACQ_FREE;
}

takeModule::takeModule(smacq_init * context) : SmacqModule(context) {
  int argc = context->argc-1;
  char ** argv = context->argv+1;;

  assert(argc==1);
  field = dts->requirefield(argv[0]);

}

