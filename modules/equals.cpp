/*
   This is a vector implementation of a equality comparisons
   between a field and a constant value.  We build a hash table of all
   the given constants and then look-up each object's field value in
   the table.  If there's a match, the hash table value tells us what
   output channel to use.

*/

#include <stdlib.h>
#include <assert.h>

#define SMACQ_MODULE_IS_VECTOR 1
#define SMACQ_MODULE_IS_STATELESS 1
#include <SmacqModule.h>
#include <FieldVec.h>
#include <vector>
#include <dts.h>

class PerType {
 public:
  dts_typeid tid;
  FieldVecHash<std::vector<int> > outChan;

  PerType(DTS * dts, dts_typeid idt, std::vector<const char*> argv)
    : tid(idt)
  {
    for (unsigned int i=0; i < argv.size(); i++) {
      DtsObject valo = dts->construct_fromstring(tid, argv[i]);
      outChan[valo].push_back(i);
    }
  }
};

typedef stdext::hash_map<dts_typeid,PerType*> TypeMap;

SMACQ_MODULE(equals,
  PROTO_CTOR(equals);
  PROTO_CONSUME();

  DtsField field;

  std::vector<const char *> argv;
  TypeMap typeSet;
);

/*
 * Check presense in set
 */
smacq_result equalsModule::consume(DtsObject datum, int & outchan) {
  DtsObject f = datum->getfield(field);
  //fprintf(stderr, "Got object %p with field %p\n", datum.get(), f.get());

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
  FieldVecHash<std::vector<int> >::iterator i = t->outChan.find(f);
  if (i != t->outChan.end()) { 
  	for (std::vector<int>::iterator oc = i->second.begin(); oc != i->second.end(); ++oc) {
                if (oc+1 == i->second.end()) { 
			// If this is the last match, then just return SMACQ_PASS
			outchan = *oc;
			return SMACQ_PASS; 
		} else {
			// Queue this match
    			enqueue(datum, *oc);
		}
        }
  }
  return SMACQ_FREE;
}

equalsModule::equalsModule(struct SmacqModule::smacq_init * context)
  : SmacqModule(context)
{
  assert(context->argc > 2);
  field = usesfield(context->argv[1]);

  argv.reserve(context->argc/2);

  /* Copy arguments without semicolons */
  for (int i = 2; i < context->argc; i++) {
	if (strcmp(context->argv[i], ";")) {
	  argv.push_back(context->argv[i]);
	}
  }
}
