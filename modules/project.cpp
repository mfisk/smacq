#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
#include <FieldVec.h>

static struct smacq_options options[] = {
  END_SMACQ_OPTIONS
};

SMACQ_MODULE(project,
  PROTO_CTOR(project);
  PROTO_CONSUME();
  PROTO_PRODUCE();

  FieldVec fieldvec;
  int empty_type;
  DtsObject product;
); 
 
smacq_result projectModule::consume(DtsObject datum, int * outchan) {
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

  product = newo;
  return (smacq_result)(SMACQ_FREE|SMACQ_PRODUCE);
}

projectModule::projectModule(struct smacq_init * context) : SmacqModule(context) {
  int argc = context->argc-1;
  char ** argv = context->argv+1;;

  assert(argc>=1);
  fieldvec.init(dts, argc, argv);

  empty_type = dts->requiretype("empty");
}

smacq_result projectModule::produce(DtsObject & datum, int * outchan) {
  if (product) {
	  datum = product;
	  product = NULL;
	  return SMACQ_PASS;
  }
  return SMACQ_END;
}

