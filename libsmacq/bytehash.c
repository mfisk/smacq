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
  int expired;
};

struct iovec_hash {
  int maxkeybytes;
  guint32 * randoms;
  GHashTable * ht;

  struct cmalloc * cm_bytes;
  struct cmalloc * cm_iovecs;
  struct cmalloc * cm_elements;
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
  s->expired = 0;

  return s;
}

static void free_element(gpointer k) {
  struct element * s = k;
  int i;
  assert(s);

  for (i=0; i<s->nvecs; i++) {
    cmfree(s->table->cm_bytes, s->iovecs[i].iov_base);
  }
  cmfree(s->table->cm_iovecs, s->iovecs);
  cmfree(s->table->cm_elements, s);
}

static void free_value(gpointer v) {
  free(v);
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
  const struct element * s = v;
  int i;
  int j;
  guint32 index = 0;

  for (i=0; i < s->nvecs; i++) {
    unsigned char * base = s->iovecs[i].iov_base;

    for (j=0; j < s->iovecs[i].iov_len; j++) {
      index += (base[j] * s->table->randoms[j % s->table->maxkeybytes]); 
    }
  }

   //    fprintf(stderr, "hash is %x\n", index); 
  return index;
}

static gint bytes_always_equal(gconstpointer v, gconstpointer v2) {
  return TRUE;
}

int bytes_mask(const struct element * b1, struct iovec * iovecs, int nvecs) {
  int i;
  

  if (b1->nvecs != nvecs) return FALSE;

  for (i=0; i<nvecs; i++) {

    if (!b1->iovecs[i].iov_len != !iovecs[i].iov_len) {

      if (!b1->iovecs[i].iov_len || !iovecs[i].iov_len) {
	break; /* Don't care */
      } else {
	return FALSE;
      }
    }

    if (memcmp(b1->iovecs[i].iov_base, iovecs[i].iov_base, b1->iovecs[i].iov_len))
      return FALSE;
  }

  return TRUE;
}

static gint bytes_equal(gconstpointer v, gconstpointer v2) {
  int i;
  const struct element * b1 = (struct element *)v;
  const struct element * b2 = (struct element *)v2;

  if (b1 == b2) return TRUE;

  if (b1->nvecs != b2->nvecs) return FALSE;

  for (i=0; i < b1->nvecs; i++) {
    if (b1->iovecs[i].iov_len != b2->iovecs[i].iov_len) return FALSE;
    if (memcmp(b1->iovecs[i].iov_base, b2->iovecs[i].iov_base, b1->iovecs[i].iov_len)) return FALSE;
  }

  return TRUE;
}

struct iovec_hash * bytes_hash_table_new(int maxbytes, enum chaining_boolean chaining, enum free_boolean dofree) {
  struct iovec_hash * myt;
  GDestroyNotify vfree;
  
  myt = g_new(struct iovec_hash, 1);
  myt->maxkeybytes = maxbytes;
  bytes_init_hash(&myt->randoms, maxbytes, 419400011);

  myt->cm_bytes = cmalloc_init(0,0);
  myt->cm_iovecs = cmalloc_init(0,0);
  myt->cm_elements = cmalloc_init(0,0);

  if (dofree == FREE) {
    vfree = free_value;
  } else {
    vfree = NULL;
  }

  if (chaining == NOCHAIN)
    myt->ht = g_hash_table_new_full(bytes_hash, bytes_always_equal, free_element, vfree);
  else 
    myt->ht = g_hash_table_new_full(bytes_hash, bytes_equal, free_element, vfree);

  return myt;
}

guint bytes_hash_valuev(struct iovec_hash * ht, int nvecs, struct iovec * vecs) {
  struct element * s = make_element(ht, vecs, nvecs);
  int val = bytes_hash(s);
  free_element(s);
  return val;
}

int bytes_hash_table_insertv(struct iovec_hash * ht, struct iovec * keys, int count, gpointer value) {
  struct element * s = make_element(ht, keys, count);
  int retval = 1;

  if (g_hash_table_lookup(ht->ht, s)) {
    retval = 0; // Old entry
  }

  g_hash_table_replace(ht->ht, s, value);

  //printf("%d\tactive table %p\n", g_hash_table_size(ht->ht), ht);

  return retval;
}

gpointer bytes_hash_table_replacev(struct iovec_hash * ht, struct iovec * keys, int count, gpointer value) {
  struct element * s = make_element(ht, keys, count);

  gpointer old = g_hash_table_lookup(ht->ht, s);
  g_hash_table_replace(ht->ht, s, value);

  return old;
}


int bytes_hash_table_incrementv(struct iovec_hash * ht, struct iovec * keys, int count) {
  struct element * s = make_element(ht, keys, count);
  gpointer current;

  current = g_hash_table_lookup(ht->ht, s);
  g_hash_table_replace(ht->ht, s, (gpointer)((int)current + 1));

  return (int)current;
}

void bytes_hash_table_insert(struct iovec_hash * ht, int keysize, unsigned char * key, gpointer value) {
  struct iovec iov;
  iov.iov_len = keysize;
  iov.iov_base = key;

  bytes_hash_table_insertv(ht, &iov, 1, value);
}
 
gpointer bytes_hash_table_lookup(struct iovec_hash * ht, int keysize, unsigned char * key) {
  struct iovec iov;
  iov.iov_len = keysize;
  iov.iov_base = key;

  return bytes_hash_table_lookupv(ht, &iov, 1);
}

gpointer bytes_hash_table_lookupv(struct iovec_hash * ht, struct iovec * vecs, int nvecs) {
  struct element * s = make_element(ht, vecs, nvecs);
  gpointer retval;

  retval = g_hash_table_lookup(ht->ht, s);
  free_element(s);
  return retval;
}

gint bytes_hash_table_lookup_extendedv(struct iovec_hash * ht, struct iovec * key, int keys, gpointer * oldkey, gpointer * current) {
  struct element * s = make_element(ht, key, keys);
  int retval;

  retval = g_hash_table_lookup_extended(ht->ht, s, oldkey, current);
  free_element(s);
  return retval;
}

gint bytes_hash_table_lookup_extended(struct iovec_hash * ht, int keysize, unsigned char * key, gpointer * oldkey, gpointer * current) {
  struct iovec iov;
  iov.iov_len = keysize;
  iov.iov_base = key;

  return bytes_hash_table_lookup_extendedv(ht, &iov, 1, oldkey, current);
}

void bytes_hash_table_foreach_remove(struct iovec_hash * ht, GHRFunc func, gpointer user_data) {
  g_hash_table_foreach_remove(ht->ht, func, user_data);
}

#ifdef FORCE_GC
static int isexpired(gpointer key, gpointer value, gpointer userdata) {
  assert(key);
  //fprintf(stderr, "glib delayed removal of value %p\n", value);
  return ((struct element*)key)->expired;
}

static inline void bytes_hash_table_gc(struct iovec_hash * ht) {
  if (! ht->gc_count--) {
    //fprintf(stderr, "doing gc\n");
    bytes_hash_table_foreach_remove(ht, isexpired, NULL);
    ht->gc_count = 1000;
  }
}
#endif

void bytes_hash_table_foreach(struct iovec_hash * ht, GHFunc func, gpointer user_data) {
  g_hash_table_foreach(ht->ht, func, user_data);
}

static int bytes_hash_table_remove(struct iovec_hash * ht, struct element * s) {
  int res;

  s->expired = 1;
  res = g_hash_table_remove(ht->ht, s);

#ifdef FORCE_GC
  // Garbage collect since g_hash_table_remove doesn't seem to work
  bytes_hash_table_gc(ht);
#endif

  return res;
}

int bytes_hash_table_removev(struct iovec_hash * ht, struct iovec * vecs, int nvecs) {
  struct element * s;
  int res;
  gpointer current;

  res = bytes_hash_table_lookup_extendedv(ht, vecs, nvecs, (gpointer*)&s, &current);
  if (!res) return 0;
  assert(s);
  s->expired = 1;

  return bytes_hash_table_remove(ht, s);
}

void bytes_hash_table_destroy(struct iovec_hash * ht) {
  g_hash_table_destroy(ht->ht);
  free(ht->randoms);
  cmalloc_destroy(ht->cm_bytes);
  cmalloc_destroy(ht->cm_iovecs);
  cmalloc_destroy(ht->cm_elements);
  free(ht);
}
