#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
#include <FieldVec.h>

static struct smacq_options options[] = {
  END_SMACQ_OPTIONS
};

SMACQ_MODULE(rename,
  PROTO_CTOR(rename);
  PROTO_CONSUME();

  FieldVec fieldvec;
); 
 
smacq_result renameModule::consume(DtsObject datum, int * outchan) {
  int i;
  int num = fieldvec.size();

  for (i = 0; i < num; i+=2) {
	DtsObject newf;
  	if (!(newf = datum->getfield(fieldvec[i]->num))) {
		fprintf(stderr, "rename: no %s field\n", 
				fieldvec[i]->name);
	}

    	datum->attach_field(fieldvec[i+1]->num, newf); 
  }

  return SMACQ_PASS;
}

renameModule::renameModule(struct smacq_init * context) : SmacqModule(context) {
  int argc = context->argc-1;
  char ** argv = context->argv+1;;

  assert(argc && ((argc%2) == 0));
  fieldvec.init(dts, argc, argv);
}

