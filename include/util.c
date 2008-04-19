#ifndef SMACQ_UTIL_C
#define SMACQ_UTIL_C

#include <stdlib.h>
#include <string>

static inline void * memdup(void * buf, int size) {
  void * copy = malloc(size);
  memcpy(copy, buf, size);
  return copy;
}

static inline void * crealloc(void * ptr, int newsize, int oldsize) {
  void * newp = realloc(ptr, newsize);
  //fprintf(stderr, "crealloc of %p from %d to %d\n", ptr, oldsize, newsize);
  if (!newp) return NULL;
  memset((unsigned char*)newp+oldsize, 0, newsize-oldsize);
  return newp;
}

static inline char * strcatn(char * dest, int len, const char * src) {
	int left = len - strlen(dest) - 1;
	return strncat(dest, src, left);
}

static inline std::string argv2string(int argc, char ** argv) {
  int i;

  std::string qstr;

  for (i=0; i<argc; i++) {
    qstr += argv[i];
    qstr += " ";
  }
  return qstr;
}


#endif
