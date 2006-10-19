#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <SmacqModule.h>
#include <bloom.h>
#include <FieldVec.h>

SMACQ_MODULE(uniq,
	     PROTO_CTOR(uniq);
	     PROTO_DTOR(uniq);
	     PROTO_CONSUME();
	     
	     FieldVec fieldvec;
	     FieldVecSet * perfectset;
	     FieldVecBloomSet * probset;
	     double prob; // Use probabilistic algorithms?
);

static struct smacq_options options[] = {
  {"m", {double_t:0}, "Max amount of memory (MB) (forces probabilistic mode)", SMACQ_OPT_TYPE_DOUBLE},
  END_SMACQ_OPTIONS
};

/*
 * Check presence in set.
 */
smacq_result uniqModule::consume(DtsObject datum, int & outchan) {
  bool isnew;

  if (prob) {
    fieldvec.getfields(datum);
    isnew = probset->insert((DtsObjectVec)fieldvec).second;
  } else {
    fieldvec.getfields(datum);
    isnew = perfectset->insert(fieldvec).second; 
  }

  if (isnew) {
    // New entry in set
    return SMACQ_PASS;
  } else {
    return SMACQ_FREE;
  }
}

uniqModule::uniqModule(smacq_init * context) 
  : SmacqModule(context), perfectset(NULL), probset(NULL)
{
  int argc;
  char ** argv = NULL;

  {
	smacq_opt prob_opt, obj_opt;

  	struct smacq_optval optvals[] = {
    		{ "m", &prob_opt}, 
    		{ "o", &obj_opt}, 
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

	prob = prob_opt.double_t;
  }

  // Consume rest of arguments as fieldnames
  fieldvec.init(dts, argc, argv);
  
  if (!prob) {
    perfectset = new FieldVecSet();
  } else {
    probset = new FieldVecBloomSet();
    // XXX: use prob to constrain memory
  }
}

uniqModule::~uniqModule() {
  delete perfectset;
  delete probset;
}

