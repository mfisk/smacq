#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "smacq.h"

static int smacqtype_string_get_string(const dts_object * o, dts_object * data) {
  *data = *o;
  return 1;
}

static int parse_string(char * buf, void ** resp, int * reslen) {
  *resp = buf;
  *reslen = strlen(buf);
    
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

