#include <SmacqModule.h>
#include <DtsObject.h>

SMACQ_MODULE(private,
  PROTO_CTOR(private);
  PROTO_CONSUME();
);

smacq_result noopModule::consume(DtsObject datum, int & outchan) {
  enqueue(datum->private_copy());
  
  return SMACQ_FREE;
}

noopModule::noopModule(struct SmacqModule::smacq_init * context) : SmacqModule(context) {
}

