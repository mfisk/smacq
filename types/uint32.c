#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "smacq.h"

static int flowtype_uint32_get_string(void * data, int dlen, void ** transform, int * tlen) {
  char buf[64]; // Only has to hold log10(2**32)

  assert(dlen==sizeof(unsigned int));

  snprintf(buf, 64, "%u", *(unsigned int*)data);
  *transform = strdup(buf);
  *tlen = strlen(data);

  return 1;
}

static int flowtype_uint32_get_double(void * data, int dlen, void ** transform, int * tlen) {
  double * d = g_new(double, 1);
  unsigned int * i = (unsigned int*)data;
  *d = *i;
  *transform = d;
  *tlen = sizeof(double);

  return 1;
}


struct dts_transform_descriptor dts_type_uint32_transforms[] = {
	{ "string",   flowtype_uint32_get_string },
	{ "double",   flowtype_uint32_get_double },
        { END,        NULL }
};

struct dts_type_info dts_type_uint32_table = {
        size: sizeof(unsigned int)
};

