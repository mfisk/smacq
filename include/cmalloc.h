struct cmalloc * cmalloc_init(int minsize, int maxfree);
void cmalloc_destroy(struct cmalloc *);
void * cmrealloc(void * data, int size);
void * cmalloc(struct cmalloc *, int size);
void cmfree(struct cmalloc *, void * base);
#define cm_new(a,b,c) ((b *)(cmalloc((a), sizeof(b)*(c))))
