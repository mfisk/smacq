#include <smacq.h>

SMACQ_MODULE(noop,
  PROTO_CTOR(noop);
  PROTO_CONSUME();
);

static struct smacq_options options[] = {
  {NULL, {string_t:NULL}, NULL, 0}
};

smacq_result noopModule::consume(DtsObject * datum, int * outchan) {
  assert(datum);

  return SMACQ_FREE;
}

noopModule::noopModule(struct smacq_init * context) : SmacqModule(context) {
}

