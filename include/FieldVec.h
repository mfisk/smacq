#ifndef FIELD_VEC_H
#define FIELD_VEC_H
#include <vector>
#include <ext/hash_map>
#include <ext/hash_set>
#include <DtsObject.h>
#include <dts.h>
#include <iov_bhash.h>

namespace stdext = ::__gnu_cxx;

class FieldVec;

/// A vector of DtsObject elements.
class DtsObjectVec : public std::vector<DtsObject> {
 public:
  DtsObjectVec::DtsObjectVec() {}
  DtsObjectVec::DtsObjectVec(FieldVec & v);
  DtsObjectVec::DtsObjectVec(DtsObject & o);

  /// Hash into value in [0..range]
  size_t hash(int seed = 0) const;

  bool masks (const DtsObjectVec &b) const;
};


/// An element of a FieldVec.
class FieldVecElement {
  friend class FieldVec;

 public:
  FieldVecElement();
  ~FieldVecElement();

  char * name;
  dts_field num;
};

/// A vector of fields from a DtsObject
class FieldVec : public std::vector<FieldVecElement*> {
 public:
  /// Initialize fields from this DtsObject
  FieldVec & getfields(DtsObject o);

  /// Return true iff one of the vector elements is undefined for this
  /// DtsObject.  Recomputed when getfields() is called.
  bool has_undefined() const;

  /// Initialize field vector from an argument vector.  Deletes any
  /// previous contents.
  void init(DTS *, int argc, char ** argv);
 
  /// Construct an empty vector.  Use init() to initialize later
  FieldVec() : dts(NULL) {};

  /// Construct and initialize field vector from an argument vector
  FieldVec(DTS * dts, int argc, char** argv) { init(dts, argc, argv); }

  /// Return a copy of the vector of current objects.
  //DtsObjectVec operator () () { return objs; }

  DtsObjectVec & getobjs() { return objs; }

  bool operator == (const FieldVec &b) const;

 private:
  DTS * dts;
  int lasttype;
  DtsObjectVec objs;

};

inline DtsObjectVec::DtsObjectVec(FieldVec & v) 
  : std::vector<DtsObject>(v.getobjs()) 
{}
  
inline DtsObjectVec::DtsObjectVec(DtsObject & o) 
  : std::vector<DtsObject>(1) 
{ 
  (*this)[0] = o; 
}

inline FieldVecElement::FieldVecElement() : num(NULL)
{}

inline FieldVecElement::~FieldVecElement() {
  if (num)
    dts_field_free(num);
}

inline void FieldVec::init(DTS * dts, int argc, char ** argv) {
  this->dts = dts;
  clear(); // Empty all elements

  while (argc-- > 0) {
    if (argv[0][0] == '-') {
      fprintf(stderr, "Unknown argument: %s\n", argv[0]);
      argv++;
      continue;
    } else {
      FieldVecElement * el = new FieldVecElement;
      this->push_back(el);
      el->name = argv[0];
      el->num = dts->requirefield(argv[0]);
      argv++;
    }
  }

  objs.resize(size());
}

inline FieldVec & FieldVec::getfields(DtsObject o) {
  FieldVecElement * el;
  FieldVec::iterator i;
  int j = 0;

  for (i = begin(); i != end(); i++) {
    el = *i;
    objs[j] = o->getfield(el->num);
    j++;
  }

  return *this;
}
    
inline bool FieldVec::has_undefined () const {
  DtsObjectVec::const_iterator i;
  
  for (i = objs.begin(); i != objs.end(); i++) {
    if (!i->get()) return true;
  }
  
  return false;
}

inline size_t DtsObjectVec::hash(const int seed) const {
  uint32_t result = seed;
  DtsObjectVec::const_iterator i;
  
  for (i = begin(); i != end(); i++) {
    result = bhash((*i)->getdata(), 
		   (*i)->getsize(), result);
  }
  
  //fprintf(stderr, "hash to %d\n", result);
  return result;
}

inline bool DtsObjectVec::masks (const DtsObjectVec &b) const {
  DtsObjectVec::const_iterator i;
  DtsObjectVec::const_iterator j;
  
  if (size() != b.size()) return false;
  if (this == &b) return true;
  
  for (i = begin(), j=b.begin(); i != end(); i++, j++) {
    if (i->get() && j->get() && (*i != *j)) {
      return false;
    }
  }
  
  return true;
}

namespace __gnu_cxx {
  template<> struct hash<DtsObjectVec> {
    size_t operator() (const DtsObjectVec & v) const { 
      return v.hash();
    }
  };
}

/// We overload the default == operator for DtsObject so that it compares
/// the objects instead of the pointers to the objects. 
template<> 
inline bool boost::operator==<DtsObject_,DtsObject_> (const DtsObject & x, const DtsObject & y) { 
	//fprintf(stderr, "o %p =? %p\n", x.get(), y.get());
	return *x == *y;
}

template <class T>
/// A hash_map (table) for FieldVec.
class FieldVecHash : public stdext::hash_map<DtsObjectVec, T> 
{
};


/// A hash_set for FieldVec.
class FieldVecSet : public stdext::hash_set<DtsObjectVec >
{
};


#endif
