/*
 * This file provides generic hash table functionality for keys that are 
 * arbitrary length byte strings.
 * 
 * We implement this using the glib hash functions and a counted byte-string
 * data structure (struct bytedata).  The API is very similar to the glib API
 * (see bytehash.h).
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bytehash.h"

struct GHashTableofBytes {
  int maxkeybytes;
  guint32 * randoms;
  GHashTable * ht;
};

static struct bytedata * make_bytesv(struct GHashTableofBytes * b, struct iovec * key, int numkeys) {
  struct bytedata * s = g_new(struct bytedata, 1);
  int size = 0;
  int offset = 0;
  int i;

  for (i=0; i<numkeys; i++) {
    size += key[i].iov_len;
  }

  s->bytes = g_malloc(size);

  for (i=0; i<numkeys; i++) {
    memcpy(s->bytes+offset, key[i].iov_base, key[i].iov_len);
    offset += key[i].iov_len;
  }
  
  s->b = b;
  s->len = size;

  return s;
}

static struct bytedata * make_bytes(struct GHashTableofBytes * b, int len, void * bytes) {
  struct bytedata * s = g_new(struct bytedata, 1);
  assert (len >= 0);
  s->len = len;
  s->bytes = g_malloc(len);
  memcpy(s->bytes, bytes, len);

  /*   fprintf(stderr, "%d bytes, %d numkeys\n", bytes, b->numkeys); */
  return s;
}

static void free_bytes(gpointer k) {
  struct bytedata * s = k;
  free(s);
}

void bytes_init_hash(guint32** randoms, int num, unsigned long prime) {
  int i;

  /* Randombytes is a an array mapping 16-bit key segments to of 32bit random numbers */
  /* There is one entry for each filter function and each allowable 16-bit key */
  /* We avoid having a full 256KB per function by using a user-supplied maximum key size */

  *randoms = g_new(guint32, num);
  for (i = 0; i < num; i++) {
    /* Modulo is probably not the right way to get them smaller than HASH_PRIME */
    (*randoms)[i] = (random() % prime);
/*       fprintf(stderr, "Random byte %d,%d is %x\n", i, j, randombytes[i][j]); */
  }
}

static guint bytes_hash(gconstpointer v) {
  const struct bytedata * s = v;
  int j;
  guint32 index = 0;

  for (j=0; j < s->len; j++) {
    index += (s->bytes[j] * s->b->randoms[j % s->b->maxkeybytes]); 
  }

   //    fprintf(stderr, "hash is %x\n", index); 
  return index;
}

static gint bytes_always_equal(gconstpointer v, gconstpointer v2) {
  return TRUE;
}

static inline int incv(const struct iovec ** vp, const void ** bp, const struct iovec * iovend) {
  *bp = *bp + 1;

  while (*bp >= ((*vp)->iov_base + (*vp)->iov_len)) {
    *vp = *vp + 1;
    if (*vp >= iovend) return 1;
    *bp = (*vp)->iov_base;
  }

  return 0;
}
    
static gint bytes_equal(gconstpointer v, gconstpointer v2) {
  const struct bytedata * b1 = (struct bytedata *)v;
  const struct bytedata * b2 = (struct bytedata *)v2;
  
  if (b1->len != b2->len) return FALSE;

  if (memcmp(b1->bytes, b2->bytes, b1->len)) return FALSE;

  return TRUE;
}

/*
struct GHashTableofBytes * bytes_hash_function_new(int keysize) {
  GHashTableofBytes * myt;
  
  myt = g_new(GHashTableofBytes, 1);
  myt->maxkeybytes = keysize;
  bytes_init_hash(&myt->randoms, keysize/2, 419400011);

  return myt;
}
*/

GHashTableofBytes * bytes_hash_table_new(int maxbytes, enum chaining_boolean chaining) {
  GHashTableofBytes * myt;
  
  myt = g_new(GHashTableofBytes, 1);
  myt->maxkeybytes = maxbytes;
  bytes_init_hash(&myt->randoms, maxbytes, 419400011);

  if (chaining == nochain)
    myt->ht = g_hash_table_new_full(bytes_hash, bytes_always_equal, free_bytes, NULL);
  else 
    myt->ht = g_hash_table_new_full(bytes_hash, bytes_equal, free_bytes, NULL);

  return myt;
}

/*
void bytes_hash_table_replaceval(GHashTableofBytes * ht, int keysize, guint16 * key, gpointer value) {
  struct bytedata * s = make_bytes(ht, keysize, key);
  struct bytedata * olds;
  gpointer current;

  if (g_hash_table_lookup_extended(ht->ht, s, (gpointer*)&olds, &current)) {
    free(olds);
    free(current);
  }

  g_hash_table_insert(ht->ht, s, value);
}
*/

guint bytes_hash_valuev(GHashTableofBytes * ht, int nvecs, struct iovec * vecs) {
  struct bytedata * s = make_bytesv(ht, vecs, nvecs);
  int val = bytes_hash(s);
  free_bytes(s);
  return val;
}

int bytes_hash_table_insertv(GHashTableofBytes * ht, struct iovec * keys, int count, gpointer value) {
  struct bytedata * s = make_bytesv(ht, keys, count);
  int retval = 1;

  if (g_hash_table_lookup(ht->ht, s)) {
    retval = 0; // Old entry
  }

  g_hash_table_replace(ht->ht, s, value);

  return retval;
}

gpointer bytes_hash_table_replacev(GHashTableofBytes * ht, struct iovec * keys, int count, gpointer value) {
  struct bytedata * s = make_bytesv(ht, keys, count);

  gpointer old = g_hash_table_lookup(ht->ht, s);
  g_hash_table_replace(ht->ht, s, value);

  return old;
}


int bytes_hash_table_incrementv(GHashTableofBytes * ht, struct iovec * keys, int count) {
  struct bytedata * s = make_bytesv(ht, keys, count);
  gpointer current;

  current = g_hash_table_lookup(ht->ht, s);
  g_hash_table_replace(ht->ht, s, current + 1);

  return (int)current;
}

void bytes_hash_table_insert(GHashTableofBytes * ht, int keysize, unsigned char * key, gpointer value) {
  struct bytedata * s = make_bytes(ht, keysize, key);
  struct bytedata * olds = NULL;
  gpointer current;

  if (g_hash_table_lookup_extended(ht->ht, s, (gpointer*)&olds, &current)) {
    //    free(olds); //XXX: This causes problems for some reason
  }

  g_hash_table_insert(ht->ht, s, value);
}

gpointer bytes_hash_table_lookup(GHashTableofBytes * ht, int keysize, unsigned char * key) {
  struct bytedata * s = make_bytes(ht, keysize, key);
  gpointer retval;

  retval = g_hash_table_lookup(ht->ht, s);
  free(s);
  return retval;
}

gpointer bytes_hash_table_lookupv(GHashTableofBytes * ht, struct iovec * vecs, int nvecs) {
  struct bytedata * s = make_bytesv(ht, vecs, nvecs);
  gpointer retval;

  retval = g_hash_table_lookup(ht->ht, s);
  free(s);
  return retval;
}

gint bytes_hash_table_lookup_extended(GHashTableofBytes * ht, int keysize, unsigned char * key, gpointer current) {
  struct bytedata * s = make_bytes(ht, keysize, key);
  int retval;
  gpointer olds;

  retval = g_hash_table_lookup_extended(ht->ht, s, &olds, current);
  free(s);
  return retval;
}

gint bytes_hash_table_lookup_extendedv(GHashTableofBytes * ht, struct iovec * key, int keys, gpointer current) {
  struct bytedata * s = make_bytesv(ht, key, keys);
  int retval;
  gpointer olds;

  retval = g_hash_table_lookup_extended(ht->ht, s, &olds, current);
  free(s);
  return retval;
}

void bytes_hash_table_foreach(GHashTableofBytes * ht, GHFunc func, gpointer user_data) {
  g_hash_table_foreach(ht->ht, func, user_data);
}

void bytes_hash_table_foreach_remove(GHashTableofBytes * ht, GHRFunc func, gpointer user_data) {
  g_hash_table_foreach_remove(ht->ht, func, user_data);
}

void bytes_hash_table_removev(GHashTableofBytes * ht, struct iovec * vecs, int nvecs) {
  struct bytedata * s = make_bytesv(ht, vecs, nvecs);

  g_hash_table_remove(ht->ht, s);
  free(s);
}

