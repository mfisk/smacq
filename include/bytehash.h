#include <sys/uio.h>
#include <stdint.h>

struct element;

enum chaining_boolean { CHAIN=0, NOCHAIN=1 };
enum free_boolean { FREE=2, NOFREE=0 };
typedef enum { false=0, true=1 } bytes_boolean;

struct iovec_hash * bytes_hash_table_new(int maxkeybytes, int flags);
int bytes_hash_table_incrementv(struct iovec_hash * ht, struct iovec *, int count);
void * bytes_hash_table_setv(struct iovec_hash * ht, struct iovec * keys, int count, void * value);
void * bytes_hash_table_lookupv(struct iovec_hash * ht, struct iovec *, int);
int bytes_hash_table_removev(struct iovec_hash * ht, struct iovec *, int);
void bytes_hash_table_destroy(struct iovec_hash * ht);
bytes_boolean bytes_hash_table_getv(struct iovec_hash * ht, struct iovec * key, int keys, struct element ** oldkey, void ** current);
void bytes_init_hash(uint32_t ** randoms, int num, unsigned long prime);
// int bytes_hash_table_get(struct iovec_hash * ht, int keysize, unsigned short * key, void * current);

uint32_t bytes_hash_valuev(struct iovec_hash * ht, int nvecs, struct iovec *);

typedef int (*ForEachFunc)(struct element * key, void * value, void * user_data);
void bytes_hash_table_foreach(struct iovec_hash * ht, ForEachFunc func, void * user_data);
void bytes_hash_table_foreach_remove(struct iovec_hash * ht, ForEachFunc func, void * user_data);

bytes_boolean bytes_mask(const struct element * b1, struct iovec * iovecs, int nvecs);
struct iovec * bytes_hash_element_iovec(struct element * e);
int bytes_hash_element_nvecs(struct element * e);
