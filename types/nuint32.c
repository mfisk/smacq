#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include "smacq.h"

static int flowtype_nuint32_get_string(void * data, int dlen, void ** transform, int * tlen) {
  char buf[64]; // Only has to hold log10(2**32)

  assert(dlen==sizeof(unsigned int));

  snprintf(buf, 64, "%u", ntohl(*(unsigned int*)data));
  *transform = strdup(buf);
  *tlen = strlen(data);

  return 1;
}

static int parse_nuint32(char * buf, void ** resp, int * reslen) {
  unsigned long * v = g_new(long, 1);
  *v = atol(buf);

  *resp = v;
  *reslen = sizeof(unsigned long);
  assert( sizeof(unsigned long) == 4);

  return 1;
}

struct dts_transform_descriptor dts_type_nuint32_transforms[] = {
  { "string",   flowtype_nuint32_get_string },
  { END,        NULL }
};

struct dts_type_info dts_type_nuint32_table = {
  size: sizeof(unsigned int),
  fromstring:parse_nuint32
};

