#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "smacq.h"

static int smacqtype_ushort_get_string(const dts_object * o, dts_object * field) {
  dts_setsize(field, 64); // Only has to hold log10(2**32)
  snprintf(field->data, 64, "%hu", dts_data_as(o, unsigned short));
  return 1;
}

static int parse_ushort(char * buf,  const dts_object * d) {
  return dts_set(d, ushort, atol(buf));
}

struct dts_field_descriptor dts_type_ushort_fields[] = {
  { "string",   "string",	smacqtype_ushort_get_string },
  { END,        NULL }
};

struct dts_type_info dts_type_ushort_table = {
  size: sizeof(unsigned short),
  fromstring: parse_ushort


};
