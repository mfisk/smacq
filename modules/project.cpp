#include <stdlib.h>
#include <math.h>
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
  DtsObject newo;
  FieldVec::iterator i;

  newo = dts->construct(empty_type, NULL);
  assert(newo);

  for (i = fieldvec.begin(); i != fieldvec.end(); i++) {
	DtsObject newf;

  	if (!(newf = datum->getfield((*i)->num))) {
	  fprintf(stderr, "project: no %s field\n", (*i)->name);
		continue;
	}
    	newo->attach_field((*i)->num, newf); 
  }
  enqueue(newo);
  return SMACQ_FREE;
}

projectModule::projectModule(struct SmacqModule::smacq_init * context) : SmacqModule(context) {
  int argc = context->argc-1;
  char ** argv = context->argv+1;;

  assert(argc>=1);
  fieldvec.init(dts, argc, argv);

  empty_type = dts->requiretype("empty");
}

