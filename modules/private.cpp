#include <SmacqModule.h>
#include <DtsObject.h>

SMACQ_MODULE(private,
  PROTO_CTOR(private);
  PROTO_CONSUME();
);

smacq_result privateModule::consume(DtsObject datum, int & outchan) {
  DtsObject o = datum->private_copy();
  //enqueue(datum->private_copy());
  enqueue(o);
  
  return SMACQ_FREE;
}

privateModule::privateModule(struct SmacqModule::smacq_init * context) : SmacqModule(context) {
}

