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

unsigned long long prime_greater_than(unsigned long long x) {
  // From: http://www.rsok.com/cgi-bin/printprimes.html
  //            http://www.rsok.com/~jrm/next_prime.html
  unsigned long long prime_list[] = {100003,      200004,      255989,     300008,    400009,   500009, 512999, 600011, 800011, 
				    1000003,      2000003,     4000037,    4194007,   6000011,  8000009,
				    10000019,     20000003,    41940023,   60000011,  80000023, 
				    100000007,    200000033,   419400011, 
				     1000000007,   1999999973,  5000000029,  5000000039, 6000000001, 7000000001, 7500000031, 8000000011,
				    10000000019,  20000000089, 50000000021,
				    100000000003, 200000000041,500000000023,
				    1000000000039,
		//		    10000000000037,
		//		    100000000000031,
		//		    1000000000000037,
				    0 };
  unsigned long y = g_spaced_primes_closest(x);
  unsigned long long * p = prime_list;

  if (x < y) return y;

  for (p = prime_list; p[1] && (p[0] < x); p++);
  if (!p[1]) 
	  	fprintf(stderr, "Warning: requested size too big!\n");

 fprintf(stderr, "Using prime %d, %llu < %llu\n", (p - prime_list) / (int)sizeof(unsigned long), x, p[0]);
  return p[0];
}

struct bloom_summary {
  guint32 * randoms;
  int numfilterfuncs;
  WORDTYPE * summary;
  WORDTYPE * count;
  unsigned long long prime;
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

static void init_hash_functions(struct bloom_summary * b) {
  bytes_init_hash(&b->randoms, b->numfilterfuncs * b->maxkeybytes, b->prime);
}

int bloom_set_hash_functions(struct bloom_summary * b, int numfilterfuncs) {
  assert(numfilterfuncs < MAXFUNCS);
  if (b->randoms) free(b->randoms);

  b->numfilterfuncs = numfilterfuncs;
  init_hash_functions(b);

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
  
  b->prime = prime_greater_than(size);
  //fprintf(stderr, "prime above %u is %u\n", size, b->prime);

  if (maxkeybytes & 1) /* is odd */ maxkeybytes++;
  b->maxkeybytes = maxkeybytes;

  bloom_set_hash_functions(b, DEFAULTNUMFUNCS);
  init_hash_functions(b);

  b->perfectsummary = bytes_hash_table_new(maxkeybytes, CHAIN, NOFREE);

  return b;
}

void bloom_make_counter(struct bloom_summary * b) {
  b->type |= BLOOM_COUNTER;
  if (b->prime >= ( pow(2,31))) {
	  fprintf(stderr, "Prime overflow!\n");
	  assert(0);
  }
  b->count = g_new(WORDTYPE, b->prime);
  fprintf(stderr, "Probabilistic counters will use %g megabytes\n",(double) b->prime * WORDBYTES / 1024 / 1024);
}
void bloom_make_summary(struct bloom_summary * b) {
  b->words = b->prime/(WORDBITS) + 1;
  if (b->prime >= (pow(2,31) * WORDBITS)) {
	  fprintf(stderr, "Prime overflow!\n");
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
  guint32 mask;

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
  int index = 0;
  int count = 0;
  int i, v;

  for (v=0; v < keys; v++) {
    for (i=0; i< key[v].iov_len; i++) {
      count++;
      index += (((unsigned char*)key[v].iov_base)[i] * b->randoms[f * b->maxkeybytes + (count % b->maxkeybytes)]); 
    }
  }

  return(index % b->prime);
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
    gpointer oldkey;
    if (bytes_hash_table_lookup_extendedv(b->perfectcount, key, keys, &oldkey, (gpointer*)&current));
    bytes_hash_table_insertv(b->perfectcount, key, keys, (gpointer)(current+1));

    if (current < smallest) {
      b->overcount++;
    }
  }

  return smallest;
}

double bloom_deviation(struct bloom_summary * b, int v) {
  return(v * b->prime / b->counttotal);
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
	bytes_hash_table_insertv(b->perfectsummary, vecs, numvecs, (gpointer)2);
      } else if (!bytes_hash_table_lookupv(b->perfectsummary, vecs, numvecs)) {
	bytes_hash_table_insertv(b->perfectsummary, vecs, numvecs, (gpointer)1);
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

  b->randoms = (guint32*)g_new(guint32, b->numfilterfuncs*b->maxkeybytes);
  if (fread(b->randoms, sizeof(guint32) * b->numfilterfuncs*b->maxkeybytes, 1, fh) < 1) {
    fprintf(stderr, "error loading sources.bitmap randomsorts\n");
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
    b->summary = g_new0(WORDTYPE, b->prime);
    if (fread(b->count, WORDBYTES * b->prime, 1, fh) < 1) {
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
  if (fwrite(b->randoms, sizeof(guint32) * b->numfilterfuncs * b->maxkeybytes, 1, fh) < 1) {
    fprintf(stderr, "error saving sources.bitmap randoms\n");
    exit(-1);
  }

  if (b->summary) {
    if (fwrite(b->summary, WORDBYTES * b->words, 1, fh) < 1) {
      fprintf(stderr, "error saving sources.bitmap summary\n");
      exit(-1);
    }
  }

  if (b->count) {
    if (fwrite(b->count, WORDBYTES * b->prime, 1, fh) < 1) {
      fprintf(stderr, "error saving sources.bitmap counters\n");
      exit(-1);
    }
  }

  fclose(fh);
}

