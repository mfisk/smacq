#include <glib.h>
#include <sys/uio.h>
#include <stdint.h>

struct element;
enum chaining_boolean { CHAIN, NOCHAIN };
enum free_boolean { FREE, NOFREE };

struct iovec_hash * bytes_hash_table_new(int maxkeybytes, enum chaining_boolean, enum free_boolean);
int bytes_hash_table_incrementv(struct iovec_hash * ht, struct iovec *, int count);
int bytes_hash_table_insertv(struct iovec_hash * ht, struct iovec *, int count,  gpointer value);
gpointer bytes_hash_table_replacev(struct iovec_hash * ht, struct iovec * keys, int count, gpointer value);
gpointer bytes_hash_table_lookupv(struct iovec_hash * ht, struct iovec *, int);
int bytes_hash_table_removev(struct iovec_hash * ht, struct iovec *, int);
void bytes_hash_table_destroy(struct iovec_hash * ht);
gint bytes_hash_table_lookup_extendedv(struct iovec_hash * ht, struct iovec * key, int keys, gpointer *oldkey, gpointer *current);
void bytes_init_hash(uint32_t** randoms, int num, unsigned long prime);
// gint bytes_hash_table_lookup_extended(struct iovec_hash * ht, int keysize, uint16_t * key, gpointer current);

uint32_t bytes_hash_valuev(struct iovec_hash * ht, int nvecs, struct iovec *);
void bytes_hash_table_foreach(struct iovec_hash * ht, GHFunc func, gpointer user_data);
void bytes_hash_table_foreach_remove(struct iovec_hash * ht, GHRFunc func, gpointer user_data);
int bytes_mask(const struct element * b1, struct iovec * iovecs, int nvecs);
struct iovec * bytes_hash_element_iovec(struct element * e);
int bytes_hash_element_nvecs(struct element * e);
