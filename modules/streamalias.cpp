#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
#include <FieldVec.h>

SMACQ_MODULE(streamalias,
  PROTO_CTOR(streamalias);
  PROTO_CONSUME();

  DtsField newfield;
  dts_typeid emptytype;
); 
 
smacq_result streamaliasModule::consume(DtsObject datum, int & outchan) {
  DtsObject newd = dts->newObject(emptytype);
  newd->attach_field(newfield, datum);
  enqueue(newd, 0);

  return SMACQ_FREE|canproduce();
}

streamaliasModule::streamaliasModule(struct SmacqModule::smacq_init * context) : SmacqModule(context) {
  int argc = context->argc-1;
  char ** argv = context->argv+1;;

  assert(argc == 1);
  newfield = dts->requirefield(argv[0]);
  emptytype = dts->requiretype("empty");
}

