/*
   This is a vector implementation of a equality comparisons
   between a field and a constant value.  We build a hash table of all
   the given constants and then look-up each object's field value in
   the table.  If there's a match, the hash table value tells us what
   output channel to use.

*/

#include <stdlib.h>
#include <assert.h>

//#define SMACQ_MODULE_IS_VECTOR 1
#define SMACQ_MODULE_IS_STATELESS 1
#include <smacq.h>
#include <FieldVec.h>
#include <FieldVec.h>
#include <vector>
#include <dts.h>

class PerType {
 public:
  dts_typeid id;
  FieldVecHash<int> outChan;

  PerType(DTS * dts, dts_typeid idt, std::vector<char*> argv)
    : id(idt)
  {
    for (unsigned int i=0; i < argv.size(); i++) {
      DtsObject valo = dts->construct_fromstring(id, argv[i]);
      outChan[valo] = i;
    }
  }
};

typedef stdext::hash_map<dts_typeid,PerType*> TypeMap;

SMACQ_MODULE(equals,
  PROTO_CTOR(equals);
  PROTO_CONSUME();

  dts_field field;

  std::vector<char *> argv;
  TypeMap typeSet;
);

/*
 * Check presense in set
 */
smacq_result equalsModule::consume(DtsObject datum, int & outchan) {
  DtsObject f = datum->getfield(field);

  if (! f) {
	return SMACQ_FREE;
  }

  PerType * t = typeSet[f->gettype()];
  if (!t) {
    t = new PerType(dts, f->gettype(), argv);
    //fprintf(stderr, "new set %p\n", t);
    typeSet[f->gettype()] = t;
  }

  //fprintf(stderr, "looking in set %p\n", t);
  FieldVecHash<int>::iterator i = t->outChan.find(f);

  if (i == t->outChan.end()) {
    return SMACQ_FREE;
  } else {
    outchan = i->second;
    return SMACQ_PASS;
  }
}

equalsModule::equalsModule(struct SmacqModule::smacq_init * context)
  : SmacqModule(context)
{
  assert(context->argc > 2);

  field = dts->requirefield(context->argv[1]);

  argv.reserve(context->argc/2);

  /* Copy arguments without semicolons */
  for (int i = 2; i < context->argc; i++) {
	if (strcmp(context->argv[i], ";")) {
	  argv.push_back(context->argv[i]);
	}
  }
}
