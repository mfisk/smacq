#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "smacq.h"

static int smacqtype_uint32_get_string(const dts_object * o, dts_object * field) {
  char buf[64]; // Only has to hold log10(2**32)

  snprintf(buf, 64, "%u", dts_data_as(o, unsigned int));
  field->data= strdup(buf);
  field->len= strlen(buf);
  field->free_data = 1;

  return 1;
}

static int smacqtype_uint32_get_double(const dts_object * o, dts_object * field) {
  double * d = g_new(double, 1);
  *d = dts_data_as(o, unsigned int);
  field->data= d;
  field->len= sizeof(double);
  field->free_data = 1;

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

