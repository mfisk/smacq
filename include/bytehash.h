#include <sys/uio.h>
#include <stdint.h>
#include <ccpp.h>

struct element;

enum chaining_boolean { CHAIN=0, NOCHAIN=1 };
enum free_boolean { FREE=2, NOFREE=0 };
#ifdef __cplusplus
typedef bool bytes_boolean;
#else
typedef enum { false=0, true=1 } bytes_boolean;
#endif


BEGIN_C_DECLS

struct iovec_hash * bytes_hash_table_new(int maxkeybytes, int flags);
int bytes_hash_table_incrementv(struct iovec_hash * ht, struct iovec *, int count);
void * bytes_hash_table_setv(struct iovec_hash * ht, struct iovec * keys, int count, void * value);
void * bytes_hash_table_setv_get(struct iovec_hash * ht, struct iovec * keys, int count, void * value, struct element ** location);
void * bytes_hash_table_lookupv(struct iovec_hash * ht, struct iovec *, int);
int bytes_hash_table_removev(struct iovec_hash * ht, struct iovec *, int);
void bytes_hash_table_remove_element(struct iovec_hash * ht, struct element * e);
void bytes_hash_table_destroy(struct iovec_hash * ht);
bytes_boolean bytes_hash_table_getv(struct iovec_hash * ht, struct iovec * key, int keys, struct element ** oldkey, void ** current);
void bytes_init_hash(uint32_t ** randoms, int num, unsigned long prime);
// int bytes_hash_table_get(struct iovec_hash * ht, int keysize, unsigned short * key, void * current);

uint32_t bytes_hashv_with(struct iovec * iovecs, int nvecs, int with);
uint32_t bytes_hashv(struct iovec * iovecs, int nvecs);

typedef int (*ForEachFunc)(struct element * key, void * value, void * user_data);
void bytes_hash_table_foreach(struct iovec_hash * ht, ForEachFunc func, void * user_data);
void bytes_hash_table_foreach_remove(struct iovec_hash * ht, ForEachFunc func, void * user_data);

bytes_boolean bytes_mask(const struct element * b1, struct iovec * iovecs, int nvecs);
struct iovec * bytes_hash_element_iovec(struct element * e);
int bytes_hash_element_nvecs(struct element * e);

static inline uint32_t bytes_hashv_into(struct iovec * vecs, int nvecs, uint32_t into) {
	return (bytes_hashv(vecs, nvecs) % into);
}
END_C_DECLS

