#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "smacq.h"

static int smacqtype_uint32_get_string(const dts_object * o, void ** transform, int * tlen) {
  char buf[64]; // Only has to hold log10(2**32)

  snprintf(buf, 64, "%u", dts_data_as(o, unsigned int));
  *transform = strdup(buf);
  *tlen = strlen(buf);

  return 1;
}

static int smacqtype_uint32_get_double(const dts_object * o, void ** transform, int * tlen) {
  double * d = g_new(double, 1);
  *d = dts_data_as(o, unsigned int);
  *transform = d;
  *tlen = sizeof(double);

  return 1;
}

struct dts_field_descriptor dts_type_uint32_fields[] = {
  { "string",   "string",	smacqtype_uint32_get_string },
  { "double",   "double",	smacqtype_uint32_get_double },
  { END,        NULL }
};

struct dts_type_info dts_type_uint32_table = {
        size: sizeof(unsigned int)
};

