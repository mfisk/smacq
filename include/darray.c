#ifndef SMACQ_DARRAY_C
#define SMACQ_DARRAY_C

#include <stdlib.h>
#include <assert.h>

struct darray {
  unsigned long * array;
  int max;
};

static inline void * darray_get(struct darray * darray, int element) {
  if (element > darray->max) {
	return NULL;
  }
  return (void*)darray->array[element];
}

static void darray_set(struct darray * darray, unsigned int element, const void * value) {
  if (element > darray->max) {
	darray->array = crealloc(darray->array, 
				 (element+1) * sizeof(unsigned long), 
				 (darray->max+1) * sizeof(unsigned long));
	darray->max = element;
  }
  darray->array[element] = (unsigned long)value;
}

static inline void darray_free(struct darray * darray) {
  free(darray->array);
  darray->array = NULL;
  darray->max = -1;
}

static inline void darray_init(struct darray * darray, int max) {
  assert(sizeof(unsigned long) == sizeof(void *));
  darray->array = calloc(max+1, sizeof(unsigned long));
  darray->max = max;
}

#endif
