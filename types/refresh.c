#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "smacq.h"

static int flowtype_refresh_get_string(void * data, int dlen, void ** transform, int * tlen) {
  *transform = strdup("REFRESH_RECORD");
  *tlen = strlen(*transform);

  return 1;
}

struct dts_transform_descriptor dts_type_refresh_transforms[] = {
	{ "string",   flowtype_refresh_get_string },
        { END,        NULL }
};

struct dts_type_info dts_type_refresh_table = {
        size: 0
};

