#include <SmacqModule.h>

SmacqModule::SmacqModule(struct smacq_init * context) {
	dts = context->dts;
}

SmacqModule::~SmacqModule() {}

smacq_result SmacqModule::consume(DtsObject * datum, int * outchan) {
	return SMACQ_ERROR;
}

smacq_result SmacqModule::produce(DtsObject ** datum, int * outchan) {
	return SMACQ_ERROR;
}

