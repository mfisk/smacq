#include <glib.h>
#include <sys/uio.h>

#define WORDTYPE guint32

extern struct bloom_summary * bloom_summary_init(int maxkeybytes, unsigned long long size);
extern struct bloom_summary * bloom_counter_init(int maxkeybytes, unsigned long long size);
extern int bloom_test_or_set(struct bloom_summary * b, unsigned char * key, 
		      int keysize, int op);
extern int bloom_test_or_setv(struct bloom_summary * b, struct iovec *, int count, int op);
extern struct bloom_summary * bloom_load(char * filename);
extern void bloom_save(char * filename, struct bloom_summary *);
int bloom_set_hash_functions(struct bloom_summary * b, int numfilterfuncs);
double bloom_get_usage(struct bloom_summary * b);
ulong bloom_get_uniq(struct bloom_summary * b);
ulong bloom_get_falsepositives(struct bloom_summary * b);
inline int TestOrSetBit(WORDTYPE * array, unsigned int number, int op);
void bloom_make_perfect(struct bloom_summary * b);
int bloom_incrementv(struct bloom_summary * b, struct iovec * key, int keys);
double bloom_deviation(struct bloom_summary * b, int);

#define BIT_SET 1
#define BIT_TEST 0
/* #define SetBit(array, number) TestOrSetBit(array, number, BIT_SET); */
#define bloom_insert(bloom, key, keysize) bloom_test_or_set(bloom, key, keysize, BIT_SET)
#define bloom_check(bloom, key, keysize) bloom_test_or_set(bloom, key, keysize, BIT_TEST)

#define bloom_insertv(bloom, vec, count) bloom_test_or_setv(bloom, vec, count, BIT_SET)
#define bloom_checkv(bloom, vec, count) bloom_test_or_setv(bloom, vec, count, BIT_TEST)


