#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "smacq.h"

static int smacqtype_bytes_get_string(const dts_object * o, dts_object * field) {
  dts_setsize(field, o->len+1);
  memcpy(dts_getdata(field), dts_getdata(o), o->len);

  /* Make sure it's NULL terminated */
  memset(field->data + o->len, '\0', 1); 

  return 1;
}

struct dts_field_spec dts_type_bytes_fields[] = {
  { "string",   "string",	smacqtype_bytes_get_string },
  { END,        NULL }
};

struct dts_type_info dts_type_bytes_table = {
	size: -1
};
