#include <glib.h>
#include <sys/uio.h>
#include <stdint.h>
#include <FieldVec.h>
#include <set>
#include <utility>
#include <vector>
#include <ext/hash_map>
 
/* Bloom filter configuration */
#define DEFAULTNUMFUNCS 4
#define DEFAULTRANGE 640000

template <class T>
  class Bloom {
 public:
  Bloom(int r) : numfilters(DEFAULTNUMFUNCS), range(r) 
  { 
  }
  
  void setfilters(int n) { numfilters = n; }
  int getfilters() { return numfilters; }
  
  void setrange(int n) { range = n; }
  int getrange() { return range; }
  
 protected:
  int numfilters;
  int range;
  
  //size_t inrange(T n, int i) { return (((*hash_fn)(n,i)) % range); }
  size_t inrange(T n, int i) { return (n.hash(i) % range); }
};

template <class T>
  class BloomSet : std::vector<bool>, Bloom<T> {
 public:
    
    BloomSet(int size = DEFAULTRANGE) : Bloom<T>(size) {}
    
    bool test(T n) {
      for (int i = 0; i < numfilters; i++) {
	if (! this[inrange(n, i)]) {
	  return false;
	}
      }
      
      return true;
    }
    
    std::pair<T,bool> insert(T n) {
		       std::pair<T,bool> p;
		       
		       bool isnew = false;	     
		       for (int i = 0; i < numfilters; i++) {
			 int x = inrange(n,i);
			 if (! this->operator[](x)) 
			   isnew = true;
			 this->operator[](x) = true;
		       }
		       
		       p.first = n;
		       p.second = isnew;
		       return p;
		     }
  };

class FieldVecBloomSet : public BloomSet<DtsObjectVec> {};

template <class T, class COUNTER>
  class BloomCounters : std::vector<COUNTER>, Bloom<T> {
 public:
    COUNTER get(T n) {
      COUNTER min = 0;
      
      for (int i = 0; i < numfilters; i++) {
	COUNTER x = (*this)[inrange(n, i)];
	if (min && (x < min)) min = x;
      }
      
      return min;
    }
    
    COUNTER increment(T n) {
      COUNTER min = get(n);
      
      for (int i =0; i < numfilters; i++) {
	if (min == (*this)[inrange(n, i)]) 
	  ++((*this)[inrange(n, i)]);
      }
      
      counttotal++;

      return (min+1);
    }
    
    double deviation(int v) {
      return (v * range / counttotal);
    }
    
    BloomCounters(int size=DEFAULTRANGE) 
      : Bloom<T>(size), counttotal(0) 
    {}
    
 private:
    unsigned long long counttotal;
    
  };



class FieldVecBloomCounters : public BloomCounters<DtsObjectVec, unsigned int> {
 public:
  FieldVecBloomCounters(int size=DEFAULTRANGE) 
    : BloomCounters<DtsObjectVec, unsigned int> (size) 
			     {}
};
