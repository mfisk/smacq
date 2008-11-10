#include <stdlib.h>
#include <assert.h>
#include <SmacqModule.h>
#include <FieldVec.h>

SMACQ_MODULE(project,
  PROTO_CTOR(project);
  PROTO_CONSUME();

  FieldVec fieldvec;
  int empty_type;
  DtsObject product;
); 
 
smacq_result projectModule::consume(DtsObject datum, int & outchan) {
  DtsObject newo = dts->construct(empty_type, NULL);

  FieldVec::iterator i;
  for (i = fieldvec.begin(); i != fieldvec.end(); i++) {
	DtsObject newf = datum->getfield((*i)->num);
	if (newf) {
    	  newo->attach_field((*i)->num, newf); 
	}
  }
  enqueue(newo);
  return SMACQ_FREE;
}

projectModule::projectModule(struct SmacqModule::smacq_init * context) : SmacqModule(context) {
  int argc = context->argc-1;
  const char ** argv = context->argv+1;;

  assert(argc>=1);
  fieldvec.init(dts, argc, argv);

  empty_type = dts->requiretype("empty");
}

