#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <SmacqModule.h>

SMACQ_MODULE(const,
  PROTO_CTOR(const);
  PROTO_CONSUME();

  DtsField field;
  DtsObject data;
);

smacq_result constModule::consume(DtsObject datum, int & outchan) {
  datum->attach_field(field, data);
  return SMACQ_PASS;
}

constModule::constModule(struct SmacqModule::smacq_init * context) : SmacqModule(context) {
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

