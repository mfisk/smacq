#include <stdlib.h>
#include <assert.h>

/* This is a "cached malloc" optimized for fixed or similar size
 * data structures */

#ifndef SMACQ_NO_OPT_CMALLOC

struct cmalloc_element {
	int size;
};

struct cmalloc {
	int minsize;
	struct cmalloc_element ** freelist; 
	struct cmalloc_element ** stackp; 
	struct cmalloc_element ** fullp; 
	struct cmalloc_element ** emptyp; 
};

struct cmalloc * cmalloc_init(int minsize, int maxfree) {
	struct cmalloc * m = calloc(sizeof(struct cmalloc), 1);
	assert(m);

	if (!minsize) minsize = 100;
	m->minsize = minsize;

	if (!maxfree) maxfree = 1000;
	m->freelist = calloc(sizeof(struct cmalloc_element *), maxfree);
	m->fullp = m->freelist + (maxfree - 1);
	m->emptyp = m->freelist - 1;
	m->stackp = m->emptyp;

	return m;
}

static inline struct cmalloc_element * cmalloc_new(int size) {
	struct cmalloc_element * e = malloc(sizeof(struct cmalloc_element) + size);
	e->size = size;
	return e+1;
}

void * cmrealloc(void * data, int size) {
	struct cmalloc_element * e = data - sizeof(struct cmalloc_element);
	if (e->size < size) {
		e = realloc(e, sizeof(struct cmalloc_element) + size);
		e->size = size;
	}
	return e+1;
}

void * cmalloc(struct cmalloc * m, int size) {
	if (m->stackp == m->emptyp) {
		return cmalloc_new(size);
	} else {
		struct cmalloc_element * e = *m->stackp;
		m->stackp--;
		if (e->size < size) {
			e = realloc(e, sizeof(struct cmalloc_element) + size);
			e->size = size;
		}
		return e+1;
	}
}

void cmfree(struct cmalloc * m, void * base) {
	assert(base != 0);
	if (m->stackp == m->fullp) {
		free(base - sizeof(struct cmalloc_element));
	} else {
		m->stackp++;
		*m->stackp = base - sizeof(struct cmalloc_element);
	}
}

void cmalloc_destroy(struct cmalloc * m) {
	while (m->stackp != m->emptyp) {
		free(m->stackp[0]);
		m->stackp--;
	}
	free(m->freelist);
	free(m);
}
#else
#warning "SMACQ_NO_OPT_CMALLOC is set"

struct cmalloc * cmalloc_init(int minsize, int maxfree) {
	return NULL;
}

void * cmrealloc(void * data, int size) {
	return realloc(data, size);
}

void * cmalloc(struct cmalloc * m, int size) {
	return malloc(size);
}

void cmfree(struct cmalloc * m, void * base) {
	free(base);
}

void cmalloc_destroy(struct cmalloc * m) {
	return;
}

#endif
