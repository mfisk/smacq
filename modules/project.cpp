#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
#include <fields.h>

static struct smacq_options options[] = {
  {NULL, {string_t:NULL}, NULL, 0}
};

SMACQ_MODULE(project,
  PROTO_CTOR(project);
  PROTO_CONSUME();
  PROTO_PRODUCE();

  struct fieldset fieldset;
  int empty_type;
  DtsObject * product;
); 
 
smacq_result projectModule::consume(DtsObject * datum, int * outchan) {
  DtsObject * newo;
  int i;

  newo = dts->construct(empty_type, NULL);
  assert(newo);

  for (i = 0; i < fieldset.num; i++) {
	DtsObject * newf;

  	if (!(newf = datum->getfield(fieldset.fields[i].num))) {
		fprintf(stderr, "project: no %s field\n", 
				fieldset.fields[i].name);
		continue;
	}
    	newo->attach_field(fieldset.fields[i].num, newf); 
  }

  product = newo;
  return (smacq_result)(SMACQ_FREE|SMACQ_PRODUCE);
}

projectModule::projectModule(struct smacq_init * context) : SmacqModule(context) {
  int argc = context->argc-1;
  char ** argv = context->argv+1;;

  assert(argc>=1);
  dts->fields_init(&fieldset, argc, argv);

  empty_type = dts->requiretype("empty");
}

smacq_result projectModule::produce(DtsObject ** datum, int * outchan) {
  if (product) {
	  *datum = product;
	  product = NULL;
	  return SMACQ_PASS;
  }
  return SMACQ_END;
}

