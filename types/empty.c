#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "smacq.h"

static int flowtype_empty_get_string(void * data, int dlen, void ** transform, int * tlen) {
  *transform = strdup("EMPTY_RECORD");
  *tlen = strlen(*transform);

  return 1;
}

struct dts_transform_descriptor dts_type_empty_transforms[] = {
	{ "string",   flowtype_empty_get_string },
        { END,        NULL }
};

struct dts_type_info dts_type_empty_table = {
        size: 0
};

