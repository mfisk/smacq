#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "smacq.h"

static int flowtype_ubyte_get_string(void * data, int dlen, void ** transform, int * tlen) {
  char buf[64]; // Only has to hold log10(2**32)

  assert(dlen==sizeof(char));

  snprintf(buf, 64, "%hhu", *(char*)data);
  *transform = strdup(buf);
  *tlen = strlen(data);

  return 1;
}

static int parse_ubyte(char * buf, void ** resp, int * reslen) {
  unsigned char * ub = g_new(unsigned char, 1);
  *ub = atol(buf);

  *resp = ub;
  *reslen = sizeof(unsigned char);

  return 1;
}

struct dts_transform_descriptor dts_type_ubyte_transforms[] = {
	{ "string",   flowtype_ubyte_get_string },
        { END,        NULL }
};

struct dts_type_info dts_type_ubyte_table = {
  size: sizeof(char),
  fromstring: parse_ubyte
};

