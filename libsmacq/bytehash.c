/*
 * This file provides generic hash table functionality for keys that are iovecs 
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <bytehash.h>
#include <cmalloc.h>
#include <iov_bhash.h>

#define STARTBUCKETS 64
#define MAXAVGCHAIN 1

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
  int num_elements;

  bytes_boolean clamped;
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


uint32_t bytes_hashv_with(struct iovec * iovecs, int nvecs, int with) {
  return iov_hash_with(iovecs, nvecs, with);
}

uint32_t bytes_hashv(struct iovec * iovecs, int nvecs) {
  return iov_hash_with(iovecs, nvecs, 0);
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
  
  myt = calloc(1, sizeof(struct iovec_hash));
  myt->maxkeybytes = maxbytes;

  myt->cm_bytes = cmalloc_init(0,0);
  myt->cm_iovecs = cmalloc_init(0,0);
  myt->cm_elements = cmalloc_init(0,0);

  myt->do_chain = !(flags & NOCHAIN); 
  myt->do_free = (flags & FREE); 

  myt->num_buckets = STARTBUCKETS;
  myt->buckets = calloc(myt->num_buckets, sizeof(struct element *));
  myt->clamped = false;
  myt->num_elements = 0;
  
  return myt;
}

static inline void chain_remove(struct element * e) {
  *(e->parent) = e->chain;
  if (e->chain) e->chain->parent = e->parent;

  e->parent = NULL;
  e->chain = NULL;
}

static inline void chain_add(struct iovec_hash * ht, int bucket, struct element * e) {
  e->chain = ht->buckets[bucket];
  if (e->chain) e->chain->parent = &(e->chain);

  e->parent = &(ht->buckets[bucket]);
  ht->buckets[bucket] = e;
}

static inline void relocate(struct iovec_hash * ht, struct element * e, int oldbucket) {
  int newbucket = bytes_hashv_into(e->iovecs, e->nvecs, ht->num_buckets);
  if (newbucket == oldbucket) return;

  /* Remove from old chain */
  chain_remove(e);

  /* Put in new chain */
  chain_add(ht, newbucket, e);
}

static void rebalance(struct iovec_hash * ht) {
  int bn;
  struct element * e, * nexte, ** newbuckets;

  //if (ht->clamped == true) return;

  /* Double size of hash table */
  newbuckets = realloc(ht->buckets, sizeof(struct element *) * ht->num_buckets * 2);
  if (newbuckets == NULL) {
	/* Unable to grow --- keep going as is */
	ht->clamped = true;
 	fprintf(stderr, "Not enough memory to grow hash table; continuing with suboptimal table size.\n");
	return;
  } else {
	/* fprintf(stderr, "Grew hash table to %d buckets\n", ht->num_buckets); */
  }

  ht->buckets = newbuckets;
  memset(ht->buckets + ht->num_buckets, 0, sizeof(struct element *) * ht->num_buckets);
  ht->num_buckets *= 2;

  /* Relocate everything in first half (the old table) */
  for (bn = 0; bn < (ht->num_buckets / 2); bn++) {
	  e = ht->buckets[bn];
	  if (e) {
		  e->parent = &(ht->buckets[bn]); /* realloc() may have changed bucket locations */
	  }
	  while (e) {
		  nexte = e->chain;
		  /* fprintf(stderr, "relocate? bucket %d e %p, next is %p\n", bn, e, nexte); */
		  relocate(ht, e, bn);
		  e = nexte;
	  }
  }
}

bytes_boolean bytes_hash_table_getv(struct iovec_hash * ht, struct iovec * vecs, int nvecs, struct element ** oldkey, void ** current) {
  struct element * bucket = ht->buckets[bytes_hashv_into(vecs, nvecs, ht->num_buckets)];

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
void * bytes_hash_table_setv_get(struct iovec_hash * ht, struct iovec * keys, int count, void * value, struct element ** location) {
  struct element * element;
  void * oldval;

  if (bytes_hash_table_getv(ht, keys, count, &element, &oldval)) {
	  element->value = value;
	  *location = element;
	  return oldval;
  } else {
	  struct element * e = make_element(ht, keys, count);
  	  int b;
	  
	  ht->num_elements++;
	  if ((ht->num_elements / ht->num_buckets) > MAXAVGCHAIN) {
		  rebalance(ht);
	  }

	  e->value = value;

	  b = bytes_hashv_into(keys, count, ht->num_buckets);
	  chain_add(ht, b, e);

	  /* fprintf(stderr, "new key %p created in bucket %d, 2nd is %p\n", e, b, e->chain);  */

	  *location = e;
	  return NULL;
  }
}

/* Return previous value, or NULL on failure */
void * bytes_hash_table_setv(struct iovec_hash * ht, struct iovec * keys, int count, void * value) {
	struct element * ignore;
	return bytes_hash_table_setv_get(ht, keys, count, value, &ignore);
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

void bytes_hash_table_remove_element(struct iovec_hash * ht, struct element * e) {
  assert(e);
  chain_remove(e);
  if (ht->do_free) free(e->value);
  free_element(e);
  ht->num_elements--;
}

void bytes_hash_table_foreach_remove(struct iovec_hash * ht, ForEachFunc func, void * user_data) {
  int bn;
  struct element * e, * nexte;

  for (bn = 0; bn < ht->num_buckets; bn++) {
	  e = ht->buckets[bn];
	  while (e) {
		  nexte = e->chain;
		  if (func(e, e->value, user_data)) {
  			bytes_hash_table_remove_element(ht, e);
		  }
		  e = nexte;
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
		e=ht->buckets[b];
		while (e) {
			nexte = e->chain;
  			bytes_hash_table_remove_element(ht, e);
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
