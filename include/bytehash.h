#include <glib.h>
#include <sys/uio.h>

struct bytedata {
  int len;
  unsigned char * bytes;
  struct GHashTableofBytes * b;
};

typedef struct GHashTableofBytes GHashTableofBytes;
enum chaining_boolean { CHAIN, NOCHAIN };
enum free_boolean { FREE, NOFREE };

GHashTableofBytes * bytes_hash_table_new(int maxkeybytes, enum chaining_boolean, enum free_boolean);
int bytes_hash_table_incrementv(GHashTableofBytes * ht, struct iovec *, int count);
int bytes_hash_table_insertv(GHashTableofBytes * ht, struct iovec *, int count,  gpointer value);
gpointer bytes_hash_table_replacev(GHashTableofBytes * ht, struct iovec * keys, int count, gpointer value);
gpointer bytes_hash_table_lookupv(GHashTableofBytes * ht, struct iovec *, int);
void bytes_hash_table_removev(GHashTableofBytes * ht, struct iovec *, int);
void bytes_hash_table_destroy(GHashTableofBytes * ht);
gint bytes_hash_table_lookup_extendedv(GHashTableofBytes * ht, struct iovec * key, int keys, gpointer current);
void bytes_init_hash(guint32** randoms, int num, unsigned long prime);
// gint bytes_hash_table_lookup_extended(GHashTableofBytes * ht, int keysize, guint16 * key, gpointer current);

guint32 bytes_hash_valuev(GHashTableofBytes * ht, int nvecs, struct iovec *);
void bytes_hash_table_foreach(GHashTableofBytes * ht, GHFunc func, gpointer user_data);
void bytes_hash_table_foreach_remove(GHashTableofBytes * ht, GHRFunc func, gpointer user_data);
