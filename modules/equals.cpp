/* 
   This is a vector implementation of a equality comparisons
   between a field and a constant value.  We build a hash table of all
   the given constants and then look-up each object's field value in
   the table.  If there's a match, the hash table value tells us what
   output channel to use.

*/

#include <stdlib.h>
#include <assert.h>
#include <smacq.h>
#include <produceq.h>
#include <FieldVec.h>
#include <IoVec.h>
#include <vector>
#include <dts.h>

//#define SMACQ_MODULE_IS_VECTOR 1
#define SMACQ_MODULE_IS_STATELESS 1

class PerType {
 public:
  dts_typeid id;
  IoVecHash<int> outChan;

  PerType(DTS * dts, dts_typeid i, int argc, std::vector<char*> argv) : id(i) {
    for (int i=0; i < argc; i++) {
      DtsObject valo = dts->construct_fromstring(id, argv[i]);
      outChan[*valo] = i-1;	
    }
  }
};

typedef stdext::hash_map<dts_typeid,PerType*> TypeMap;

SMACQ_MODULE(equals,
  PROTO_CTOR(equals);
  PROTO_CONSUME();
  PROTO_PRODUCE();

  dts_field field;

  int argc;
  std::vector<char *> argv;

  struct smacq_outputq * outputq;
  TypeMap typeSet;
);

/*
 * Check presense in set
 */
smacq_result equalsModule::consume(DtsObject datum, int * outchan) {
  IoVecHash<int>::iterator i;
	
  DtsObject f = datum->getfield(field);
  if (! f) 
  	return SMACQ_FREE;

  PerType * t = typeSet[f->gettype()];
  if (!t) {
    t = new PerType(dts, f->gettype(), argc, argv);
    typeSet[f->gettype()] = t;
  }

  i = t->outChan.find(*f);

  if (i == t->outChan.end()) {
	return SMACQ_FREE;
  } else {
	*outchan = i->second;
 	return SMACQ_PASS;
  }
}

equalsModule::equalsModule(struct smacq_init * context) : SmacqModule(context) {
  int j = 0;
  assert(context->argc > 2);
  
  field = dts->requirefield(context->argv[1]);

  /* Copy arguments without semicolons */
  for (int i = 2; i < context->argc; i++) {
	if (strcmp(context->argv[i], ";")) {
	  argv.push_back(context->argv[i]);
	  //	argv[j] = context->argv[i];
	  j++;
	} 
  }
  argc = j;
}

smacq_result equalsModule::produce(DtsObject & datum, int * outchan) {
  return smacq_produce_dequeue(&outputq, datum, outchan);
}
                                                                                                                                 
