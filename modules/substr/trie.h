#ifndef TRIE_H
#define TRIE_H

struct trie {
  void * target;
  unsigned char * val;
  short len;
  struct trie ** branch;

  struct trie * fail; /* Aho-Corasick only */
};

typedef struct trieobj {
	int direction;
	int collapse;
        int alphabet;
	struct trie ** mtries;
} trieobj;

#define TRIE_REVERSE 1
#define TRIE_FORWARD 0

trieobj * trieobj_new(int direction, int collapse, int alphabet);

int trie_insert(trieobj * self, unsigned char * str, int len, void * target);

void trie_dump(trieobj * self);
void trie_nocase(trieobj * self);

#include "substr.h"

void * trie_find(trieobj * self, unsigned char * str, int len,  int * left, struct trie ** trie);

#endif
