#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include "smacq.h"

static int smacqtype_nuint32_get_string(const dts_object * o, void ** transform, int * tlen) {
  char buf[64]; // Only has to hold log10(2**32)

  snprintf(buf, 64, "%u", ntohl(dts_data_as(o, unsigned int)));
  *transform = strdup(buf);
  *tlen = strlen(buf);

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

struct dts_field_descriptor dts_type_nuint32_fields[] = {
  { "string",   "string",	smacqtype_nuint32_get_string },
  { END,        NULL }
};

struct dts_type_info dts_type_nuint32_table = {
  size: sizeof(unsigned int),
  fromstring:parse_nuint32
};

