#ifndef SUBSTR_H
#define SUBSTR_H

#include <assert.h> // for assert()

#if defined(WIN32) && !defined(inline)
    /* Visual C++ uses the keyword "__inline" rather than "inline" */
    #define inline __inline
#endif

enum substr_type { 
  SUBSTR_HORSPOOL,
  SUBSTR_AC,
  SUBSTR_SETBMH,
  SUBSTR_BM,
  SUBSTR_FAST
};

struct substr_search_resume {
  int offset;
  struct trie * trie;
  
  /* Set: */
  int left;
  int len;
};

struct substr_search_result {
  struct pattern * p;
  int shift;

  struct substr_search_resume resume;
};

#include "trie.h"

struct pattern {
  int len;
  void * handle; /* Opaque ID used by calling program */
  unsigned char * pattern;
  struct pattern * next;
  int nocase;

  int * bad; 
  int * good;
};

typedef struct ruleset {
  /* Only needed in head, could use separate struct for efficiency */
  int searchtype;
  int shortest;
  int maxdepth;
  int minoffset;
  struct pattern * patterns;
  int (*search)(struct ruleset * head, unsigned char * hay, int len,
	  struct substr_search_result * stat);
  int (*compile)(struct ruleset * head);
  trieobj * trie;

  /* Set only: */
  int * bad;
} substr_object;

int substr_add(struct ruleset * rs, int len, unsigned char * str, int nocase, void * handle, int offset, int depth);
int substr_compile(struct ruleset * rs);
struct ruleset * substr_new(enum substr_type  searchtype);

static inline int
substr_search(substr_object * obj, unsigned char * hay, int len,
	      struct substr_search_result * stat)
{
  if (obj->maxdepth && (obj->maxdepth < len)) {
    /* Chris Green removed this assignment: */
    len = obj->maxdepth;
  }

  //assert(obj->search);
  return obj->search(obj, 
		     hay + obj->minoffset, 
		     len - obj->minoffset, 
		     stat);
}

/*
#define substr_search(rs, hay, len, stat) \
        ((rs)->search((rs), (hay), (len), (stat)))
*/

#endif

