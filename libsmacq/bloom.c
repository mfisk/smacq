#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "bloom.h"
#include "bytehash.h"

/* Bloom filter configuration */
#define DEFAULTNUMFUNCS 4

/* Architecture tuning */
#define WORDBYTES (sizeof(WORDTYPE))
#define WORDBITS (WORDBYTES*8)

#define BLOOM_SUMMARY 1
#define BLOOM_COUNTER 2
#define BLOOM_PERFECT 4

#define MAXFUNCS 10

struct bloom_summary {
  int numfilterfuncs;
  WORDTYPE * summary;
  WORDTYPE * count;
  unsigned long long size;
  int words;
  int maxkeybytes;
  unsigned long bitsset;
  unsigned long uniq;
  int type;

  /* Data structures for non-probabilisitic (BLOOM_PERFECT) implementation */
  struct iovec_hash * perfectsummary, * perfectcount;

  /* Diagnostic values for comparing probablisitic and perfect techniques */
  unsigned long falsepositives, overcount;

  /* Values for incrank (deviation) functions */
  unsigned long counttotal;
};

int bloom_set_hash_functions(struct bloom_summary * b, int numfilterfuncs) {
  assert(numfilterfuncs < MAXFUNCS);
  b->numfilterfuncs = numfilterfuncs;

  return numfilterfuncs;
}

double bloom_get_usage(struct bloom_summary * b) {
  return ((double)b->bitsset / ((double)b->words * WORDBITS));
}

unsigned long bloom_get_falsepositives(struct bloom_summary * b) {
  return (b->falsepositives);
}

unsigned long bloom_get_uniq(struct bloom_summary * b) {
  return ((double)b->uniq);
}

static struct bloom_summary * bloom_init(int maxkeybytes, unsigned long long size) {
  struct bloom_summary * b = g_new0(struct bloom_summary, 1);
  b->size = size;
  
  if (maxkeybytes & 1) /* is odd */ maxkeybytes++;
  b->maxkeybytes = maxkeybytes;

  bloom_set_hash_functions(b, DEFAULTNUMFUNCS);

  b->perfectsummary = bytes_hash_table_new(maxkeybytes, CHAIN|NOFREE);

  return b;
}

void bloom_make_counter(struct bloom_summary * b) {
  b->type |= BLOOM_COUNTER;
  b->count = g_new(WORDTYPE, b->size);
  fprintf(stderr, "Probabilistic counters will use %g megabytes\n",(double) b->size * WORDBYTES / 1024 / 1024);
}
void bloom_make_summary(struct bloom_summary * b) {
  b->words = b->size/(WORDBITS) + 1;
  if (b->size >= (pow(2,31) * WORDBITS)) {
	  fprintf(stderr, "Table too big!\n");
	  assert(0);
  }
  fprintf(stderr, "Bloom summary will be %g megabytes\n",(double) b->words * WORDBYTES / 1024 / 1024);

  b->type |= BLOOM_SUMMARY;
  b->summary = g_new(WORDTYPE, b->words);
}
void bloom_make_perfect(struct bloom_summary * b) {
  b->type |= BLOOM_PERFECT;
}

struct bloom_summary * bloom_summary_init(int maxkeybytes, unsigned long long size) {
  struct bloom_summary * b = bloom_init(maxkeybytes, size);
  bloom_make_summary(b);
  return(b);
}

struct bloom_summary * bloom_counter_init(int maxkeybytes, unsigned long long size) {
  struct bloom_summary * b = bloom_init(maxkeybytes, size);
  bloom_make_counter(b);
  return(b);
}

/* Return whether the bit was previously set */
int TestOrSetBit(WORDTYPE * array, unsigned int number, int op) {
  uint32_t mask;

  mask = (1 << ((number % WORDBITS)-1));
  //fprintf(stderr, "\tmaps to %u\t%u\n", number/WORDBITS, number %WORDBITS);
  if (mask & array[number / WORDBITS]) {
    return(1);
  }
  if (op == BIT_SET) {
    array[number/WORDBITS] |= mask;
  }
  return 0;
}

int even(int x) {
  return (!(x&1));
}

static inline int applyfilterfunc(struct bloom_summary * b, int f, struct iovec * key, int keys) {
  return(bytes_hashv_with(key, keys, f));
}

/* Return value is the value before being incremented */
int bloom_incrementv(struct bloom_summary * b, struct iovec * key, int keys) {
  int f;
  unsigned long indices[MAXFUNCS];
  int smallest = -1;

  assert(b->count);

  /* Find which counter(s) are the lowest */
  for (f=0; f < b->numfilterfuncs; f++) {
    indices[f] = applyfilterfunc(b, f, key, keys);

    if ((smallest < 0) || (b->count[indices[f]] < smallest)) {
      smallest = b->count[indices[f]];
    }
  }

  /* Increment those lowest counters */
  for (f=0; f < b->numfilterfuncs; f++) {
    if (b->count[indices[f]] == smallest) {
      /* fprintf(stderr, "%d (%ld); ", f, indices[f]); */
      b->count[indices[f]]++;
      b->counttotal++;
    }
  }  
  /* intf(stderr, "has smallest (%d)\n", smallest); */

  if (b->type & BLOOM_PERFECT) {
    int current = 0;
    struct element * oldkey;
    if (bytes_hash_table_getv(b->perfectcount, key, keys, &oldkey, (gpointer*)&current));
    bytes_hash_table_setv(b->perfectcount, key, keys, (gpointer)(current+1));

    if (current < smallest) {
      b->overcount++;
    }
  }

  return smallest;
}

double bloom_deviation(struct bloom_summary * b, int v) {
  return(v * b->size / b->counttotal);
}

int bloom_increment(struct bloom_summary * b, unsigned char *key, int keysize) {
  struct iovec v;
  v.iov_base = key;
  v.iov_len = keysize;

  return(bloom_incrementv(b, &v, 1));
}

/* Return 0 if the key was already present, otherwise number of missing bits */
int bloom_test_or_setv(struct bloom_summary * b, struct iovec * vecs, int numvecs, int op) {
  int * table = b->summary;
  int retval = 0;
  int f;

  for (f=0; f < b->numfilterfuncs; f++) {
    retval += TestOrSetBit(table, 
			   applyfilterfunc(b, f, vecs, numvecs),
			   op);
  }

  if (op == BIT_SET) {
    int diff = (b->numfilterfuncs - retval);
    if (diff) {
      b->bitsset += diff;
      b->uniq++;
    }

    if (b->type & BLOOM_PERFECT) {
      if (diff) {
	bytes_hash_table_setv(b->perfectsummary, vecs, numvecs, (gpointer)2);
      } else if (!bytes_hash_table_lookupv(b->perfectsummary, vecs, numvecs)) {
	bytes_hash_table_setv(b->perfectsummary, vecs, numvecs, (gpointer)1);
	b->falsepositives++;
      }
    }
  }
  return (b->numfilterfuncs - retval); /* Number of bits that weren't set */
}

int bloom_test_or_set(struct bloom_summary * b, unsigned char *key, int keysize, int op) {
  struct iovec v;
  v.iov_base = key;
  v.iov_len = keysize;

  return bloom_test_or_setv(b, &v, 1, op);
}

struct bloom_summary * bloom_load(char * filename) {
  struct bloom_summary * b;

  FILE* fh = fopen(filename, "r");
  if (!fh) {
    fprintf(stderr, "error opening sources.bitmap.  Starting empty\n");
    return NULL;
  }
  b = g_new(struct bloom_summary, 1);
  if (fread(b, sizeof(struct bloom_summary), 1, fh) < 1) {
    fprintf(stderr, "error loading sources.bitmap object\n");
    exit(-1);
  }

  if (b->summary) {
    b->summary = g_new0(WORDTYPE, b->words);
    if (fread(b->summary, WORDBYTES * b->words, 1, fh) < 1) {
      fprintf(stderr, "error loading sources.bitmap summary\n");
      exit(-1);
    }
  }

  if (b->count) {
    b->summary = g_new0(WORDTYPE, b->size);
    if (fread(b->count, WORDBYTES * b->size, 1, fh) < 1) {
      fprintf(stderr, "error loading sources.bitmap count\n");
      exit(-1);
    }
  }

  fclose(fh);

  assert(b->numfilterfuncs < MAXFUNCS);
  return(b);
}

void bloom_save(char * filename, struct bloom_summary * b) {
  FILE* fh = fopen(filename, "w");
  
  if (fwrite(b, sizeof(struct bloom_summary), 1, fh) < 1) {
    fprintf(stderr, "error saving sources.bitmap object\n");
    exit(-1);
  }

  if (b->summary) {
    if (fwrite(b->summary, WORDBYTES * b->words, 1, fh) < 1) {
      fprintf(stderr, "error saving sources.bitmap summary\n");
      exit(-1);
    }
  }

  if (b->count) {
    if (fwrite(b->count, WORDBYTES * b->size, 1, fh) < 1) {
      fprintf(stderr, "error saving sources.bitmap counters\n");
      exit(-1);
    }
  }

  fclose(fh);
}

