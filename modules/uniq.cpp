#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <SmacqModule.h>
#include <bloom.h>
#include <FieldVec.h>

/* Really it's an ulonglong, but STL doesn't come with a hash function
   for that, so we're lazy and truncate to ulong. */
typedef unsigned int idtype;

class IdSet : public stdext::hash_set<idtype> {};

SMACQ_MODULE(uniq,
	     PROTO_CTOR(uniq);
	     PROTO_DTOR(uniq);
	     PROTO_CONSUME();
	     
	     FieldVec fieldvec;
	     FieldVecSet * perfectset;
	     FieldVecBloomSet * probset;
	     IdSet * idset;
	     double prob; // Use probabilistic algorithms?
	     int use_obj_id;
);

static struct smacq_options options[] = {
  {"m", {double_t:0}, "Max amount of memory (MB) (forces probabilistic mode)", SMACQ_OPT_TYPE_DOUBLE},
  {"o", {boolean_t:0}, "Use object ID instead of fields", SMACQ_OPT_TYPE_BOOLEAN},
  END_SMACQ_OPTIONS
};

/*
 * Check presence in set.
 */
smacq_result uniqModule::consume(DtsObject datum, int & outchan) {
  bool isnew;

  if (use_obj_id) {
    // Danger: truncating long long to long
    idtype id = (idtype)datum->getid();
    isnew = idset->insert(id).second;
  } else if (prob) {
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

uniqModule::uniqModule(struct SmacqModule::smacq_init * context) 
  : SmacqModule(context), perfectset(NULL), probset(NULL), idset(NULL)
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
	use_obj_id = obj_opt.boolean_t;
  }

  // Consume rest of arguments as fieldnames
  fieldvec.init(dts, argc, argv);

  if (use_obj_id) {
    idset = new IdSet;
  } else if (!prob) {
    perfectset = new FieldVecSet();
  } else {
    probset = new FieldVecBloomSet();
    // XXX: use prob to constrain memory
  }
}

uniqModule::~uniqModule() {
  delete perfectset;
  delete probset;
  delete idset;
}

