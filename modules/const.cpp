#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <smacq.h>

SMACQ_MODULE(const,
  PROTO_CTOR(const);
  PROTO_DTOR(const);
  PROTO_CONSUME();

  dts_field field;
  DtsObject data;
);

static struct smacq_options options[] = {
  {NULL, {NULL}, NULL, 0}
};

smacq_result constModule::consume(DtsObject datum, int * outchan) {
  assert(datum);

  
  datum->attach_field(field, data);

  return SMACQ_PASS;
}

constModule::constModule(struct smacq_init * context) : SmacqModule(context) {
  int argc = context->argc-1;
  char ** argv = context->argv+1;

  assert(argc<=2);
  assert(argc>=1);

  data = dts->construct_fromstring(dts->requiretype("string"), strdup(argv[0]));
  if (argc == 2) {
  	field = dts->requirefield(argv[1]);
  } else {
  	field = dts->requirefield("const");
  }
}

constModule::~constModule() {
  dts_field_free(field);
}

