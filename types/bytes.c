#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "smacq.h"

static int smacqtype_bytes_get_string(const dts_object * o, dts_object * field) {
  char buf[64]; // Only has to hold log10(2**32)

  snprintf(buf, 64, "data at %p", dts_getdata(o));
  field->data= strdup(buf);
  field->len= strlen(buf);
  field->free_data = 1;

  return 1;
}

struct dts_field_descriptor dts_type_bytes_fields[] = {
  { "string",   "string",	smacqtype_bytes_get_string },
  { END,        NULL }
};

struct dts_type_info dts_type_bytes_table = {
	size: -1
};
