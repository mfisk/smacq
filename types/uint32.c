#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "smacq.h"

static int smacqtype_uint32_get_string(const dts_object * o, dts_object * field) {
  dts_setsize(field, 64); // Only has to hold log10(2**32)
  snprintf(field->data, 64, "%u", dts_data_as(o, unsigned int));
  return 1;
}

static int smacqtype_uint32_get_hexstring(const dts_object * o, dts_object * field) {
  dts_setsize(field, 16); // 4 bytes -> 8 chars + 3 = 11
  snprintf(field->data, 64, "0x%x", dts_data_as(o, unsigned int));
  return 1;
}

static int smacqtype_uint32_get_double(const dts_object * o, dts_object * field) {
  double d = dts_data_as(o, unsigned int);
  return dts_set(field, double, d);
}

struct dts_field_spec dts_type_uint32_fields[] = {
  { "string",   "string",	smacqtype_uint32_get_string },
  { "string",   "hexstring",	smacqtype_uint32_get_hexstring },
  { "double",   "double",	smacqtype_uint32_get_double },
  { END,        NULL }
};

struct dts_type_info dts_type_uint32_table = {
        size: sizeof(unsigned int)
};

