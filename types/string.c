#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "smacq.h"

static int smacqtype_string_get_string(const dts_object * o, dts_object * data) {
  dts_setsize(data, strlen(o->data));
  strcpy(data->data, o->data);
  return 1;
}

static int parse_string(char * buf,  const dts_object * d) {
  dts_setsize(d, strlen(buf));
  strcpy(d->data, buf);
    
  return 1;
}

struct dts_field_descriptor dts_type_string_fields[] = {
  { "string",   "string",  smacqtype_string_get_string },
  { END,        NULL }
};

struct dts_type_info dts_type_string_table = {
  size: -1,
  fromstring: parse_string
};

