#ifndef IOVEC_HASH_H
#define IOVEC_HASH_H
#include <stdint.h>
#include <iov_bhash.h>
#include <vector>
#include <ext/hash_map>
#include <ext/hash_set>

namespace stdext = ::__gnu_cxx;

class IoVecElement {
 public:
  unsigned char * iov_base;
  int iov_len;

  bool operator== (const IoVecElement& x) const;
};

inline bool IoVecElement::operator==(const IoVecElement & j) const {
  if (iov_len != j.iov_len) 
    return false;
  
  return (! memcmp(iov_base, j.iov_base, iov_len));
}

class IoVec : public std::vector<IoVecElement> {
 public:
  IoVec(size_type n) : std::vector<IoVecElement>(n) {}
  IoVec() {}

  size_t hash(const int seed=0) const {
    uint32_t result = seed;
    IoVec::const_iterator i = begin();
    
    for (; i != end(); i++) {
      result = bhash(i->iov_base, i->iov_len, result);
    }

    return result;
  }
    
  bool masks (const IoVec &b) const {
    IoVec::const_iterator i;
    IoVec::const_iterator j;

    if (size() != b.size()) return false;
    if (this == &b) return true;
    
    for (i = begin(), j=b.begin(); i != end(); i++, j++) {
      if ((i->iov_len != 0 && j->iov_len != 0) && (i != j)) {
	return false;
      }
    }
     
    return true;
  }

  bool operator == (const IoVec&b) {
    IoVec::const_iterator i;
    IoVec::const_iterator j;

    if (size() != b.size()) return false;
    if (this == &b) return true;
    
    for (i = begin(), j=b.begin(); i != end(); i++, j++) {
      if (i != j) return false;
    }
     
    return true;
  }
};

/*
template<> size_t hash<IoVec>(IoVec*i) {
  i->hash();
}
*/

class eq_iovec {
 public:
  bool operator() (const IoVec& a, const IoVec& b) const {
    return (a == b);
  }
};

class hash_iovec { 
 public:
  size_t operator() (const IoVec & a, const int seed = 0) const {
    return a.hash(seed);
  }
};

template <class T>
class IoVecHash : public stdext::hash_map<IoVec, T, hash_iovec> {};

class IoVecSet : public stdext::hash_set<IoVec, hash_iovec>{};

#endif
