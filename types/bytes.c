#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "smacq.h"

static int smacqtype_bytes_get_string(const dts_object * o, dts_object * field) {
  field->data = o->data;
  field->len = o->len;

  return 1;
}

struct dts_field_descriptor dts_type_bytes_fields[] = {
  { "string",   "string",	smacqtype_bytes_get_string },
  { END,        NULL }
};

struct dts_type_info dts_type_bytes_table = {
	size: -1
};
