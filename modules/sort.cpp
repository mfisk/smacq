#include <SmacqModule.h>
#include <FieldVec.h>

static struct smacq_options options[] = {
  {"b", {double_t:0}, "Batch size", SMACQ_OPT_TYPE_DOUBLE},
  {"r", {boolean_t:0}, "Reverse order", SMACQ_OPT_TYPE_BOOLEAN},
  END_SMACQ_OPTIONS
};

DtsField doublefield;

namespace std {
	template<>
	class less<FieldVec> {
		public:
			bool operator() (const FieldVec &a, const FieldVec &b) {
				DtsObjectVec ao = a.getobjs();
				DtsObjectVec bo = b.getobjs();
				DtsObjectVec::const_iterator i,j;

				for (i=ao.begin(), j=bo.begin(); i!=ao.end() && j!=bo.end(); ++i, ++j) {
					// Compare an element in the vector.
					// Return true iff less-than, false iff greater-than
					// Iff equal, then continue through vector.

					if (!*i && !*j) {
						continue; // ==
					} else if (!*i) {
						return true;
					} else if (!*j) {
						return false;
					} else {
						// Try to do a numeric comparison first 
						DtsObject ifo, jfo;
						ifo = (*i)->getfield(doublefield, true);
						jfo = (*j)->getfield(doublefield, true);

						if (ifo && jfo) {
							double di, dj;
							di = dts_data_as(ifo, double);
							dj = dts_data_as(jfo, double);
							//fprintf(stderr, "sort using doubles %g <? %g -> %d\n", di, dj, di < dj);
							if (di < dj) { 
								return true;
							} else if (di > dj) {
								return false;
							}
						} else {
							// If all else fails, just do a memcmp()
							int ilen = (*i)->getsize();
							int jlen = (*j)->getsize();
							int cmp = memcmp((*i)->getdata(), (*j)->getdata(), min(ilen, jlen));
							if (cmp < 0 || (cmp == 0 && ilen < jlen)) {
								return true;
							} else if (cmp > 0) {
								return false;
							}
						}
					}
				}
				
				// In case we get here, but the vectors are different lengths
				// The shorter one is "less" than the longer one
				if (j != ao.end()) return true;

				return false;
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
  bool reverse_sort;

  void empty_tree();
); 


sortModule::sortModule(smacq_init * context) 
  : SmacqModule(context)
{
  int argc = 0;
  char ** argv;

    smacq_opt batch_opt, reverse_opt;
    
    struct smacq_optval optvals[] = {
      { "b", &batch_opt}, 
      { "r", &reverse_opt}, 
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

  // Use options
  reverse_sort = reverse_opt.boolean_t;
  batch_size = (unsigned long)batch_opt.double_t;
  count = 0;
  
  // Consume rest of arguments as fieldnames
  fieldvec.init(dts, argc, argv);

  doublefield = dts->requirefield("double");
}

void sortModule::empty_tree() {
  if (!reverse_sort) {
    tree_t::iterator i;
    for (i=tree.begin(); i!=tree.end(); ++i) {
	  enqueue((*i).second);
    }
  } else {
    tree_t::reverse_iterator i;
    for (i=tree.rbegin(); i!=tree.rend(); ++i) {
	  enqueue((*i).second);
    }
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
