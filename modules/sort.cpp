#include <SmacqModule.h>
#include <FieldVec.h>

static struct smacq_options options[] = {
  {"b", {double_t:0}, "Batch size", SMACQ_OPT_TYPE_DOUBLE},
  END_SMACQ_OPTIONS
};

namespace std {
	template<>
	class less<FieldVec> {
		public:
			bool operator() (const FieldVec &a, const FieldVec &b) {
				DtsObjectVec ao = a.getobjs();
				DtsObjectVec bo = b.getobjs();
				DtsObjectVec::const_iterator i,j;
				for (i=ao.begin(), j=bo.begin(); i!=ao.end() && j!=bo.end(); ++i, ++j) {
					int len = min((*i)->getsize(), (*j)->getsize());
					if (memcmp((*i)->getdata(), (*j)->getdata(), len) >= 0) {
						return false;
					}
				}
				
				// In case vectors are different lengths
				if (i != ao.end()) return false;

				return true;
			}
	};
};

SMACQ_MODULE(sort, 
  PROTO_CTOR(sort);
  PROTO_DTOR(sort);
  PROTO_CONSUME();

  FieldVec fieldvec;

  unsigned long batch_size;
  unsigned long count;

  typedef std::multimap<FieldVec, DtsObject> tree_t;
  tree_t tree;

protected:
	void empty_tree();
); 

sortModule::sortModule(struct SmacqModule::smacq_init * context) 
  : SmacqModule(context)
{
  int argc = 0;
  char ** argv;

    smacq_opt batch_opt;
    
    struct smacq_optval optvals[] = {
      { "b", &batch_opt}, 
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

  // Use batch_opt
  batch_size = (unsigned long)batch_opt.double_t;
  count = 0;
  
  // Consume rest of arguments as fieldnames
  fieldvec.init(dts, argc, argv);
}

void sortModule::empty_tree() {
  tree_t::iterator i;
  for (i=tree.begin(); i!=tree.end(); ++i) {
	  enqueue((*i).second);
  }
  tree.clear();
}

smacq_result sortModule::consume(DtsObject datum, int & outchan) {
  fieldvec.getfields(datum);
  tree.insert(std::pair<FieldVec, DtsObject>(fieldvec, datum));

  if (batch_size && (++count == batch_size)) {
	  empty_tree();
	  count = 0;
  }
  return SMACQ_FREE;
}

sortModule::~sortModule() {
	empty_tree();
}
