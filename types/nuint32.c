#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include "smacq.h"

static int smacqtype_nuint32_get_string(const dts_object * o, dts_object * field) {
  dts_setsize(field, 64); // Only has to hold log10(2**32)
  snprintf(field->data, 64, "%u", ntohl(dts_data_as(o, unsigned int)));
  return 1;
}

static int parse_nuint32(char * buf,  const dts_object * d) {
  return dts_set(d, unsigned long, ntohl(atol(buf)));
}

struct dts_field_descriptor dts_type_nuint32_fields[] = {
  { "string",   "string",	smacqtype_nuint32_get_string },
  { END,        NULL }
};

struct dts_type_info dts_type_nuint32_table = {
  size: sizeof(unsigned int),
  fromstring:parse_nuint32
};

