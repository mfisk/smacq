#include <stdio.h>
#include <StrucioStream.h>
#include <SmacqModule.h>

SMACQ_MODULE(cflow,
  PROTO_CTOR(cflow);
  PROTO_DTOR(cflow);
  PROTO_CONSUME();
  PROTO_PRODUCE();

  int cflow_type;
  StrucioStream * fh;
);

smacq_result cflowModule::produce(DtsObject & datum, int & outchan) {
  if (fh && (datum = fh->construct(dts, cflow_type))) {
		return SMACQ_PASS|SMACQ_PRODUCE;
	} else {
		return SMACQ_FREE;
  }
}

smacq_result cflowModule::consume(DtsObject datum, int & outchan) {
  if (fh) delete fh;
  fh = StrucioStream::MagicOpen(datum);
  return SMACQ_FREE|SMACQ_PRODUCE;
}

cflowModule::cflowModule(smacq_init * context) : SmacqModule(context), fh(NULL) {
 SmacqFileModule(context);

 cflow_type = dts->requiretype("cflow");
}

cflowModule::~cflowModule() {
 if (fh) delete fh;
}
