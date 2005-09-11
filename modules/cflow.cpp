#include <stdio.h>
#include <StrucioStream.h>
#include <SmacqModule.h>

SMACQ_MODULE(cflow,
  PROTO_CTOR(cflow);
  PROTO_CONSUME();

  int cflow_type;
);

smacq_result cflowModule::consume(DtsObject datum, int & outchan) {
  StrucioStream * fh = StrucioStream::MagicOpen(datum);
  DtsObject o;
  if (fh) {
	while ((o = fh->construct(dts, cflow_type))) {
		enqueue(o);
	}
  }
  return SMACQ_FREE;
}

cflowModule::cflowModule(struct SmacqModule::smacq_init * context) : SmacqModule(context) {
 SmacqFileModule(context);

 cflow_type = dts->requiretype("cflow");
}
