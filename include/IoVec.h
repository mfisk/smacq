#ifndef IOVEC_HASH_H
#define IOVEC_HASH_H
#include <stdint.h>
#include <iov_bhash.h>
#include <vector>
#include <ext/hash_map>
#include <ext/hash_set>

namespace stdext = ::__gnu_cxx;

/// An element of an FieldVec.  Describes a memory region.
class FieldVecElement {
 public:
  /// A pointer to the beginning of the memory region
  unsigned char * iov_base;

  /// The size of the memory region
  int iov_len;

  bool operator== (const FieldVecElement& x) const;
  bool operator!= (const FieldVecElement& x) const { return !((*this)==(x)); }
};

inline bool FieldVecElement::operator==(const FieldVecElement & j) const {
  /*
  fprintf(stderr, "iovecelement %p + %d == %p + %d\n", 
	  iov_base, iov_len, 
	  j.iov_base, j.iov_len);
  */
  if (iov_len != j.iov_len) 
    return false;
  
  return (! memcmp(iov_base, j.iov_base, iov_len));
}

typedef FieldVec<FieldVecElement> FieldVec;

/// A vector of FieldVecElement pointing to various sized byte ranges.
template <class T>
class FieldVec : public std::vector<T> {
 public:
  FieldVec(size_type n) : std::vector<T>(n) {}
  FieldVec() : std::vector<T>() {}

};

namespace __gnu_cxx {
   template<> struct hash<FieldVec> {
     size_t operator() (const FieldVec & v) const { 
       return v.hash();
     }
   };
}

/*
namespace std {
   template<> struct equal_to<FieldVec> {
     bool operator() (const FieldVec &a, const FieldVec &b) const {
       return a==b;
     }
   };
}
*/
/// A hash_map (table) of FieldVec keys.
template <class T>
class FieldVecHash : public stdext::hash_map<FieldVec, T> {};

/// A hash_set of FieldVec keys.
class FieldVecSet : public stdext::hash_set<FieldVec, stdext::hash<FieldVec>, std::equal_to<FieldVec> > {};

#endif
