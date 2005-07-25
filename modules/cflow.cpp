#include <stdio.h>
#include <StrucioStream.h>
#include <SmacqModule.h>

SMACQ_MODULE(cflow,
  PROTO_CTOR(cflow);
  PROTO_CONSUME();

  int argc;
  char ** argv;

  int cflow_type;
  int cflow_type_size;
);

smacq_result cflowModule::consume(DtsObject datum, int & outchan) {
  StrucioStream * fh = StrucioStream::Open(datum);
  DtsObject o;
  if (fh) {
	while ((o = fh->construct(dts, cflow_type))) {
		enqueue(o);
	}
  }
  return SMACQ_FREE;
}

cflowModule::cflowModule(struct SmacqModule::smacq_init * context) : SmacqModule(context) {
 StrucioStream::FileModule(context);

 cflow_type = dts->requiretype("cflow");
}
