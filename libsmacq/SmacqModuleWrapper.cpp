#include <SmacqModuleWrapper.h>
#include <smacq-dataflow.h>

smacq_result SmacqModuleWrapper::produce(DtsObject ** datum, int * outchan) {
     return ops.produce(state, datum, outchan);
}

smacq_result SmacqModuleWrapper::consume(DtsObject * datum, int * outchan) {
     return ops.consume(state, datum, outchan);
}

SmacqModuleWrapper::~SmacqModuleWrapper() {
     ops.shutdown(state);
}

SmacqModuleWrapper::SmacqModuleWrapper(smacq_graph * f, smacq_init* context) : SmacqModule(context) {
     ops = f->ops;

     if ((SMACQ_END|SMACQ_ERROR) & ops.init(context)) {
        fprintf(stderr, "Error initializing module %s\n", f->name);
	assert(0);
     }

     state = (struct state*)context->state;
}


