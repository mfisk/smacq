/*
 * This file provides generic hash table functionality for keys that are iovecs 
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <bytehash.h>
#include <cmalloc.h>

struct bytevec {
  int len;
  unsigned char * data;
};

struct element {
  struct iovec * iovecs;
  int nvecs;

  struct iovec_hash * table;
  struct element * chain;
  struct element ** parent;
  void * value;
};

struct iovec_hash {
  int do_chain, do_free;
  int maxkeybytes;
  uint32_t * randoms;

  struct cmalloc * cm_bytes;
  struct cmalloc * cm_iovecs;
  struct cmalloc * cm_elements;

  struct element ** buckets;
  int num_buckets;
};

struct iovec * bytes_hash_element_iovec(struct element * e) {
  return e->iovecs;
}

int bytes_hash_element_nvecs(struct element * e) {
  return e->nvecs;
}

static struct element * make_element(struct iovec_hash * b, struct iovec * iovecs, int nvecs) {
  struct element * s = cm_new(b->cm_elements, struct element, 1);
  int i;

  s->iovecs = cm_new(b->cm_iovecs, struct iovec, nvecs);
  s->nvecs = nvecs;
  
  for (i=0; i < nvecs; i++) {
    s->iovecs[i].iov_base = cm_new(b->cm_bytes, char, iovecs[i].iov_len);
    s->iovecs[i].iov_len = iovecs[i].iov_len;
    memcpy(s->iovecs[i].iov_base, iovecs[i].iov_base, iovecs[i].iov_len);
  }

  s->table = b;

  return s;
}

static void free_element(void * k) {
  struct element * s = k;
  int i;
  assert(s);

  for (i=0; i<s->nvecs; i++) {
    cmfree(s->table->cm_bytes, s->iovecs[i].iov_base);
  }
  cmfree(s->table->cm_iovecs, s->iovecs);
  cmfree(s->table->cm_elements, s);
}

void bytes_init_hash(uint32_t** randoms, int num, unsigned long prime) {
  int i;

  /* Randombytes is a an array mapping 16-bit key segments to of 32bit random numbers */
  /* There is one entry for each filter function and each allowable 16-bit key */
  /* We avoid having a full 256KB per function by using a user-supplied maximum key size */

  *randoms = malloc(sizeof(uint32_t)*num);
  assert(*randoms);

  for (i = 0; i < num; i++) {
    /* Modulo is probably not the right way to get them smaller than HASH_PRIME */
    (*randoms)[i] = (random() % prime);
/*       fprintf(stderr, "Random byte %d,%d is %x\n", i, j, randombytes[i][j]); */
  }
}

static unsigned int bytes_iovec_hash(struct iovec_hash * table, struct iovec * iovecs, int nvecs) {
  int i;
  int j;
  uint32_t index = 0;

  for (i=0; i < nvecs; i++) {
    unsigned char * base = iovecs[i].iov_base;

    for (j=0; j < iovecs[i].iov_len; j++) {
      index += (base[j] * table->randoms[j % table->maxkeybytes]); 
    }
  }

   //    fprintf(stderr, "hash is %x\n", index); 
  return index;
}

bytes_boolean bytes_mask(const struct element * b1, struct iovec * iovecs, int nvecs) {
  int i;
  
  if (b1->nvecs != nvecs) return false;

  for (i=0; i<nvecs; i++) {

    if (!b1->iovecs[i].iov_len != !iovecs[i].iov_len) {

      if (!b1->iovecs[i].iov_len || !iovecs[i].iov_len) {
	break; /* Don't care */
      } else {
	return false;
      }
    }

    if (memcmp(b1->iovecs[i].iov_base, iovecs[i].iov_base, b1->iovecs[i].iov_len))
      return false;
  }

  return true;
}

static bytes_boolean iovecs_equal(struct iovec * v1, int n1, struct iovec * v2, int n2) {
  int i;

  if (n1 != n2) return false;
  if (v1 == v2) return true;

  for (i=0; i < n1; i++) {
    if (v1[i].iov_len != v2[i].iov_len) return false;
    if (memcmp(v1[i].iov_base, v2[i].iov_base, v1[i].iov_len)) return false;
  }

  return true;
}

struct iovec_hash * bytes_hash_table_new(int maxbytes, int flags) {
  struct iovec_hash * myt;
  
  myt = malloc(sizeof(struct iovec_hash));
  myt->maxkeybytes = maxbytes;
  bytes_init_hash(&myt->randoms, maxbytes, 419400011);

  myt->cm_bytes = cmalloc_init(0,0);
  myt->cm_iovecs = cmalloc_init(0,0);
  myt->cm_elements = cmalloc_init(0,0);

  myt->do_chain = !(flags & NOCHAIN); 
  myt->do_free = (flags & FREE); 

  myt->num_buckets = 1000; /* XXX */
  myt->buckets = calloc(sizeof(struct element *), 1000);
  
  return myt;
}

uint32_t bytes_hash_valuev(struct iovec_hash * ht, int nvecs, struct iovec * vecs) {
  int val = bytes_iovec_hash(ht, vecs, nvecs);
  return val;
}

bytes_boolean bytes_hash_table_getv(struct iovec_hash * ht, struct iovec * vecs, int nvecs, struct element ** oldkey, void ** current) {
  struct element * bucket = ht->buckets[bytes_iovec_hash(ht, vecs, nvecs) % ht->num_buckets];

  if (bucket && ht->do_chain) {
     struct element * e;
     for (e = bucket; e; e = e->chain) {
	if (iovecs_equal(vecs, nvecs, e->iovecs, e->nvecs) == true) break;
     }
     /* fprintf(stderr, "Checking bucket %p resulted in %p\n", bucket, e); */
     bucket = e;
  }

  *oldkey = bucket;

  if (bucket) {
     *current = bucket->value;
     return true;
  } else {
     *current = NULL; 
     return false;
  }
}

/* Return previous value, or NULL on failure */
void * bytes_hash_table_setv(struct iovec_hash * ht, struct iovec * keys, int count, void * value) {
  struct element * element;
  void * oldval;

  if (bytes_hash_table_getv(ht, keys, count, &element, &oldval)) {
	  element->value = value;
	  return oldval;
  } else {
	  struct element * e = make_element(ht, keys, count);
  	  int b = bytes_iovec_hash(ht, keys, count) % ht->num_buckets;

  	  e->parent = &(ht->buckets[b]);
  	  e->chain = ht->buckets[b];
	  if (e->chain) e->chain->parent = &(e->chain);
	  e->value = value;
  	  ht->buckets[b] = e;

	  /* Do not free element here and only here. */

	  /* fprintf(stderr, "new key created in bucket %d\n", b); */
	  return NULL;
  }
}

int bytes_hash_table_incrementv(struct iovec_hash * ht, struct iovec * keys, int count) {
  struct element * element;
  int oldval;

  if (bytes_hash_table_getv(ht, keys, count, &element, (void**)&oldval)) {
  	element->value = (void*)(oldval+1);
  	return (int)oldval;
  } else {
	return -1;
  }
   
}

void bytes_hash_table_set(struct iovec_hash * ht, int keysize, unsigned char * key, void * value) {
  struct iovec iov;
  iov.iov_len = keysize;
  iov.iov_base = key;

  bytes_hash_table_setv(ht, &iov, 1, value);
}

void * bytes_hash_table_lookup(struct iovec_hash * ht, int keysize, unsigned char * key) {
  struct iovec iov;
  iov.iov_len = keysize;
  iov.iov_base = key;

  return bytes_hash_table_lookupv(ht, &iov, 1);
}

void * bytes_hash_table_lookupv(struct iovec_hash * ht, struct iovec * vecs, int nvecs) {
  struct element * oldkey;
  void * value; 

  if (bytes_hash_table_getv(ht, vecs, nvecs, &oldkey, &value)) {
	  return value;
  } else { 
	  return NULL;
  }
}

int bytes_hash_table_get(struct iovec_hash * ht, int keysize, unsigned char * key, struct element ** oldkey, void ** current) {
  struct iovec iov;
  iov.iov_len = keysize;
  iov.iov_base = key;

  return bytes_hash_table_getv(ht, &iov, 1, oldkey, current);
}

static inline void bytes_hash_table_remove_element(struct iovec_hash * ht, struct element * e) {
  assert(e->parent);
  *(e->parent) = e->chain;
  free_element(e);
}

void bytes_hash_table_foreach_remove(struct iovec_hash * ht, ForEachFunc func, void * user_data) {
  int bn;
  struct element * e;

  for (bn = 0; bn < ht->num_buckets; bn++) {
	  for (e = ht->buckets[bn]; e; e = e->chain) {
		  if (func(e, e->value, user_data)) {
  			bytes_hash_table_remove_element(ht, e);
		  }
	  }
  }
}

void bytes_hash_table_foreach(struct iovec_hash * ht, ForEachFunc func, void * user_data) {
  int bn;
  struct element * e;

  for (bn = 0; bn < ht->num_buckets; bn++) {
	  for (e = ht->buckets[bn]; e; e = e->chain) {
		  func(e, e->value, user_data);
	  }
  }
}

int bytes_hash_table_removev(struct iovec_hash * ht, struct iovec * vecs, int nvecs) {
  struct element * s;
  int res;
  void * current;

  res = bytes_hash_table_getv(ht, vecs, nvecs, &s, &current);

  if (res == false) return 0;

  assert(s);
  bytes_hash_table_remove_element(ht, s);
  return 1;
}

void bytes_hash_table_destroy(struct iovec_hash * ht) {
  {
  	int b;
  	struct element * e, * nexte;
	
  	for (b=0; b < ht->num_buckets; b++) {
		for (e=ht->buckets[b]; e; ) {
			nexte = e->chain;
			if (ht->do_free) free(e->value);
			free_element(e);
			e = nexte;
		}
  	}
  }

  free(ht->randoms);
  cmalloc_destroy(ht->cm_bytes);
  cmalloc_destroy(ht->cm_iovecs);
  cmalloc_destroy(ht->cm_elements);
  free(ht);
}
