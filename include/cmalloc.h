#ifndef CMALLOC_H
#define CMALLOC_H

#include <ccpp.h>

BEGIN_C_DECLS

struct cmalloc * cmalloc_init(int minsize, int maxfree);
void cmalloc_destroy(struct cmalloc *);
void * cmrealloc(void * data, int size);
void * cmalloc(struct cmalloc *, int size);
void cmfree(struct cmalloc *, void * base);
#define cm_new(a,b,c) ((b *)(cmalloc((a), sizeof(b)*(c))))

END_C_DECLS

#endif
