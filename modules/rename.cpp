#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
#include <fields.h>

static struct smacq_options options[] = {
  {NULL, {string_t:NULL}, NULL, 0}
};

SMACQ_MODULE(rename,
  PROTO_CTOR(rename);
  PROTO_CONSUME();

  struct fieldset fieldset;
); 
 
smacq_result renameModule::consume(DtsObject * datum, int * outchan) {
  int i;

  for (i = 0; i < fieldset.num; i+=2) {
	DtsObject * newf;
  	if (!(newf = datum->getfield(fieldset.fields[i].num))) {
		fprintf(stderr, "rename: no %s field\n", 
				fieldset.fields[i].name);
	}

    	datum->attach_field(fieldset.fields[i+1].num, newf); 
  }

  return SMACQ_PASS;
}

renameModule::renameModule(struct smacq_init * context) : SmacqModule(context) {
  int argc = context->argc-1;
  char ** argv = context->argv+1;;

  assert(argc && ((argc%2) == 0));
  dts->fields_init(&fieldset, argc, argv);
}

