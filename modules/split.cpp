#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <SmacqModule.h>
#include <FieldVec.h>
#include <FieldVec.h>


enum mode { ROUND_ROBIN, UNIQUE, BUCKET };

static struct smacq_options options[] = {
  {"b", {int_t:0}, "Number of ways to split", SMACQ_OPT_TYPE_INT},
  END_SMACQ_OPTIONS
};

SMACQ_MODULE(split,
  PROTO_CTOR(split);
  PROTO_CONSUME();

  FieldVec fieldvec;
  int children;
  int bucket;
  enum mode mode;
  SmacqGraphNode_ptr self;
  FieldVecHash<int> hashtable;
); 

smacq_result splitModule::consume(DtsObject datum, int & outchan) {
  int bucket;
  fieldvec.getfields(datum);
	
  if (mode == ROUND_ROBIN) {
    // Round-robin
    this->bucket = ((this->bucket)  % children);
    bucket = this->bucket;
  } else if (mode == BUCKET) {
    bucket = fieldvec.hash() % children;
  } else if (mode == UNIQUE) {
    bucket = hashtable[fieldvec];
    if (!bucket) {
	SmacqGraphNode_ptr newClone;
	bucket = bucket++;
	//fprintf(stderr, "Cloning %d\n", bucket);
        newClone = self->clone_tree(NULL);
	newClone->init(dts);

	// 0 return value is error, so everything is inflated by 1
    	hashtable[fieldvec] = bucket;
    } else { 
	// 0 return value is error, so everything is inflated by 1
	bucket--;
    }
  } else {
    assert(0);
  }

  // bucket = (bucket++) % children;
  //fprintf(stderr, "Sending to output #%d\n", bucket);
  outchan = bucket;
  return SMACQ_PASS;
}

splitModule::splitModule(smacq_init * context)
  : SmacqModule(context),
    bucket(0)
{
  int argc = 0;
  char ** argv;
  int i;
  self = context->self;

  {
	smacq_opt buckets;

  	struct smacq_optval optvals[] = {
    		{"b", &buckets},
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

	children = buckets.int_t;
  }

  // Consume rest of arguments as fieldnames
  if (argc) {
  	if (children) {
		fprintf(stderr, "Cannot specify both -b and fields\n");
		assert(0);
	}

  	mode = UNIQUE;
  	fieldvec.init(dts, argc, argv);
  } else if (!children) {
	fprintf(stderr, "Must specify either -b or fields\n");
	assert(0);
  } else {
	mode = ROUND_ROBIN;

  	for  (i=1; i < children; i++) {
	  context->self->clone_tree(NULL);
  	}
  }
}

