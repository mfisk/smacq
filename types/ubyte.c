#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "smacq.h"

static int smacqtype_ubyte_get_string(const dts_object * o, dts_object * field) {
  char buf[64]; // Only has to hold log10(2**32)

  snprintf(buf, 64, "%hhu", dts_data_as(o, char));
  field->data= strdup(buf);
  field->len= strlen(buf);
  field->free_data = 1;

  return 1;
}

static int parse_ubyte(char * buf, void ** resp, int * reslen) {
  unsigned char * ub = g_new(unsigned char, 1);
  *ub = atol(buf);

  *resp = ub;
  *reslen = sizeof(unsigned char);

  return 1;
}

struct dts_field_descriptor dts_type_ubyte_fields[] = {
  { "string",   "string",	smacqtype_ubyte_get_string },
  { END,        NULL }
};

struct dts_type_info dts_type_ubyte_table = {
  size: sizeof(char),
  fromstring: parse_ubyte
};

