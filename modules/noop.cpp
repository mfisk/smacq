#include <SmacqModule.h>

SMACQ_MODULE(noop,
  PROTO_CTOR(noop);
  PROTO_CONSUME();
);

smacq_result noopModule::consume(DtsObject datum, int & outchan) {
  return SMACQ_FREE;
}

noopModule::noopModule(struct SmacqModule::smacq_init * context) : SmacqModule(context) {
}

