#include <string.h>
#include <SmacqModule.h>

// This module is used when evaluating Boolean filters in parallel.
// It passes objects unless it has already passed this object.
// State is managed by attaching a field to the object.
// Keeping a local table would use unbounded memory.
SMACQ_MODULE(uniqobj,
	     PROTO_CTOR(uniqobj);
	     PROTO_CONSUME();

	     DtsField seen_field;
	     DtsObject seen_obj;
);

smacq_result uniqobjModule::consume(DtsObject datum, int & outchan) {
  DtsObject foo = datum->getfield(seen_field, true);
  if (foo) {
    return SMACQ_FREE;
  } else {
    datum->attach_field(seen_field, seen_obj);
    return SMACQ_PASS;
  }
}

uniqobjModule::uniqobjModule(struct SmacqModule::smacq_init * context) 
  : SmacqModule(context)
{
  char seen_fieldstr[64];
  sprintf(seen_fieldstr, "_or%p", this);
  seen_field = dts->requirefield(seen_fieldstr);
  seen_obj = dts->newObject(dts->requiretype("empty"));
}

