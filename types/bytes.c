#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "smacq.h"

static int smacqtype_bytes_get_string(void * data, int dlen, void ** transform, int * tlen) {
  char buf[64]; // Only has to hold log10(2**32)

  assert(dlen=sizeof(int));

  snprintf(buf, 64, "%p", data);
  *transform = strdup(buf);
  *tlen = strlen(data);

  return 1;
}

struct dts_transform_descriptor dts_type_bytes_transforms[] = {
	{ "string",   smacqtype_bytes_get_string },
        { END,        NULL }
};

struct dts_type_info dts_type_bytes_table = {
	size: -1
};
