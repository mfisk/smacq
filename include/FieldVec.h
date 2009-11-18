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
  DtsObjectVec() {}
  DtsObjectVec(FieldVec & v);
  DtsObjectVec(DtsObject & o);

  /// Hash vector
  size_t hash(int seed = 0) const;

  /// Hash vector in an order-independent way
  size_t sethash(int seed = 0) const;

  bool masks (const DtsObjectVec &b) const;
};


/// An element of a FieldVec.
class FieldVecElement {
  friend class FieldVec;

 public:
  const char * name;
  DtsField num;
};

/// A vector of fields from a DtsObject
class FieldVec : public std::vector<FieldVecElement*> {
 public:
  /// Initialize fields from this DtsObject.  Return true if one or more fields present.
  bool getfields(DtsObject o, bool nowarn = false);

  /// Return true iff one of the vector elements is undefined for this
  /// DtsObject.  Recomputed when getfields() is called.
  bool has_undefined() const;

  /// Initialize field vector from an argument vector.  Deletes any
  /// previous contents.
  void init(DTS *, int argc, const char ** argv);
 
  /// Construct an empty vector.  Use init() to initialize later
  FieldVec() : dts(NULL) {};

  /// Construct and initialize field vector from an argument vector
  FieldVec(DTS * dts, int argc, const char** argv) { init(dts, argc, argv); }

  /// Return a copy of the vector of current objects.
  //DtsObjectVec operator () () { return objs; }

  DtsObjectVec & getobjs() { return objs; }
  const DtsObjectVec & getobjs() const { return objs; }

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

inline void FieldVec::init(DTS * dts, int argc, const char ** argv) {
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

inline bool FieldVec::getfields(DtsObject o, bool nowarn) {
  FieldVecElement * el;
  FieldVec::iterator i;
  bool hasfields = false;
  int j = 0;

  for (i = begin(); i != end(); i++) {
    el = *i;
    objs[j] = o->getfield(el->num, nowarn);
    if (objs[j]) hasfields = true;
    j++;
  }

  return hasfields;
}
    
inline bool FieldVec::has_undefined () const {
  DtsObjectVec::const_iterator i;
  
  for (i = objs.begin(); i != objs.end(); i++) {
    if (!i->get()) return true;
  }
  
  return false;
}

inline size_t DtsObjectVec::sethash(const int seed) const {
  uint32_t result = seed;
  DtsObjectVec::const_iterator i;
  
  for (i = begin(); i != end(); ++i) {
    if (*i) {
	// This version alwasy hashes with the seed and then just XORs
	// the results
       result += bhash((*i)->getdata(), 
		   (*i)->getsize(), seed);
    }
  }
  
  //fprintf(stderr, "hash to %xld\n", result);
  return result;
}

inline size_t DtsObjectVec::hash(const int seed) const {
  uint32_t result = seed;
  DtsObjectVec::const_iterator i;
  
  for (i = begin(); i != end(); ++i) {
    if (*i) {
       result = bhash((*i)->getdata(), 
		   (*i)->getsize(), result);
    }
  }
  
  //fprintf(stderr, "hash to %xld\n", result);
  return result;
}

/// Return true iff argument vector mask fits this vector.  
inline bool DtsObjectVec::masks (const DtsObjectVec &b) const {
  DtsObjectVec::const_iterator i;
  DtsObjectVec::const_iterator j;
  
  if (size() != b.size()) {
	fprintf(stderr, "Warning: Masking with vector of different size!\n");
	return false;
  }
  if (this == &b) { 
	fprintf(stderr, "Warning: Masking with self!\n");
	return true;
  }
  
  for (i = begin(), j=b.begin(); i != end(); i++, j++) {
    // i & j point to DtsObjects; IFF both are specified, see if they are equal (overloaded)
    if (i->get() && j->get() && (i->get() != j->get())) {
      return false;
    }
  }
  
  return true;
}

#include <openssl/sha.h>

/// A scalar hash of a DtsObjectVec.
class DtsDigest {
 public:
  /// Copy construct a hash
  DtsDigest(const DtsDigest & d) { 
	memcpy(val, d.val, SHA_DIGEST_LENGTH);
  }

  /// Construct a hash from a FieldVec.
  DtsDigest(FieldVec & v) { 
	_from(v.getobjs()); 
  }

  /// Construct a hash from a DtsObjectVec.
  DtsDigest(const DtsObjectVec & v) { 
	_from(v); 
  }

  /// Construct a hash from a single DtsObject.
  DtsDigest(DtsObject & o) {
	/// XXX. this is wasteful and lame.
	DtsObjectVec v(o);
	_from(v);
  }

  bool operator< (const DtsDigest &y) const {
	return (memcmp(val, y.val, SHA_DIGEST_LENGTH) < 0);
  }

  bool operator==(const DtsDigest &y) const {
	return (!memcmp(val, y.val, SHA_DIGEST_LENGTH));
  }

 private:
  /// Compute the hash value from a DtsObjectVec.
  void _from(const DtsObjectVec & v) {
    DtsObjectVec::const_iterator i;
  
    SHA_CTX c;
    SHA1_Init(&c); 

    for (i = v.begin(); i != v.end(); ++i) {
      SHA1_Update(&c, (*i)->getdata(), (*i)->getsize());
    }

    SHA1_Final(val, &c);
  }

  unsigned char val[SHA_DIGEST_LENGTH];
};

namespace __gnu_cxx {

  /* 
  // This is only necessary if doing a hash_map, hash_set, etc. of DtsDigest objects 
  template<> struct hash<DtsDigest> {
    size_t operator() (const DtsDigest & v) const {
      return v.hash();
    }
  };
  */

  // This is necessary for doing an STL hash_map, hash_set, etc. of DtsObjectVec objects 
  template<> struct hash<DtsObjectVec> {
    size_t operator() (const DtsObjectVec & v) const { 
      return v.hash();
    }
  };
}

namespace boost {
  // This is in namespace boost because DtsObject is a boot::intrusive_ptr<DtsObject_>

  /// We overload the default == operator for DtsObject so that it compares
  /// the objects instead of the pointers to the objects.   
  /// Without this, there are no compile-time errors, but you still need it.  
  template<> 
  inline bool operator==<DtsObject_,DtsObject_> (const DtsObject & x, const DtsObject & y) { 
	//fprintf(stderr, "o %p =? %p\n", x.get(), y.get());
	return *x == *y;
  }

  template<> 
  inline bool operator<(DtsObject const & a, DtsObject const & b)
  {
    return *a < *b;
  }
}

/// A dictionary for FieldVec keys.
template <class T>
class FieldVecDict : public std::map<DtsObjectVec, T>  {
};

/// A map for FieldVec keys.  The key itself is not stored, just a hash.
template <class T>
class FieldVecHash : public std::map<DtsDigest, T> 
{
};

/// A hash_set for FieldVec.
class FieldVecSet : public std::set<DtsDigest >
{
};

#endif
