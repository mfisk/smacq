#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#define LOOPSIZE 1e7

extern "C" void virtual_bench(int);

extern "C" int function_bench(int);


class Meta
{
public:
  virtual int OtherValue(int i) const;
  virtual int Value(int i) const;

};

int Meta::OtherValue(int i) const {
  assert(0);
}

int Meta::Value(int i) const {
  assert(0);
}

class Real : public Meta
{
public:
  int Value(int i) const;
};

int Real::Value(int i) const {
  return i;
}

int function_bench(int x) {
  return x;
}
  
void virtual_bench(int base) {
  Meta * r;
  double val;
  double len;
  long sec, usec;
  struct timeval tvstart, tvstop;
  int i;

  if (base) {
    r = new Meta;
  } else {
    r = new Real;
  }

  gettimeofday(&tvstart, NULL);
 
  for (i=0; i<LOOPSIZE; i++) { 
    val = r->Value(42);
  }

  gettimeofday(&tvstop, NULL);

  sec = tvstop.tv_sec - tvstart.tv_sec;
  usec = tvstop.tv_usec - tvstart.tv_usec;
  if (usec < 0) {
	  sec--;
	  usec += (long)1e6;  
  }
  
  len = sec + (usec * 1e-6);

  fprintf(stderr, "%g ns/virtual\n", len*1e9/LOOPSIZE	  );

}
