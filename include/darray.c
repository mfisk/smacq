#ifndef SMACQ_DARRAY_C
#define SMACQ_DARRAY_C

#include <stdlib.h>
#include <assert.h>
#include <string.h>

struct darray {
  unsigned long * array;
  int max;
};

static inline void * darray_get(struct darray * darray, unsigned int element) {
  if ((int)element > darray->max) {
	return NULL;
  }
  return (void*)darray->array[element];
}

static void darray_set(struct darray * darray, unsigned int element, const void * value) {
  if ((int)element > darray->max) {
	darray->array = (long unsigned int*)crealloc(darray->array, 
				 (element+1) * sizeof(unsigned long), 
				 (darray->max+1) * sizeof(unsigned long));
	darray->max = element;
  }
  darray->array[element] = (unsigned long)value;
}

static inline int darray_append(struct darray * darray, const void * value) {
  darray_set(darray, darray->max+1, value);
  return darray->max;
}

static inline void darray_free(struct darray * darray) {
  free(darray->array);
  darray->array = NULL;
  darray->max = -2;
}

static inline void darray_init(struct darray * darray, int num) {
  assert(num >= 0);
  assert(sizeof(unsigned long) == sizeof(void *));
  if (num) {
    darray->array = (long unsigned int*)calloc(num, sizeof(unsigned long));
  }
  darray->max = num-1;
}

static inline int darray_size(struct darray* darray) {
  return(darray->max + 1);
}

#include <string.h>

typedef struct darray idset;

/*
struct idset {
  int size;
  struct darray * strs;
};
*/

static inline void idset_init(idset * idset) {
  darray_init(idset, 0);
}

static inline int str2id_try(idset * ids, char * str) {
  int i;

  /* Slow linear search */
  for (i=0; i <= ids->max; i++) {
    char * cand = (char*)ids->array[i];
    /* fprintf(stderr, "comparing %s and %s\n", str, cand); */
    if (cand && (!strcmp(str, cand))) {
      /* fprintf(stderr, "found %s in %s %d\n", str, cand, i); */
      return i;
    }
  }

  return -1;
}

static inline int str2id(idset * ids, char * str) {
  int i = str2id_try(ids, str);
  if (i == -1) {
    return darray_append(ids, strdup(str));
  } else {
    return i;
  }
}  

#endif
