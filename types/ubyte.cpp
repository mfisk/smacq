#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <dts-module.h>

static int smacqtype_ubyte_get_string(DtsObject * o, DtsObject * field) {
  field->setsize(64); // Only has to hold log10(2**32)
  snprintf((char*)field->getdata(), 64, "%hhu", dts_data_as(o, char));
  return 1;
}

static int parse_ubyte(char * buf,  DtsObject * d) {
  return dts_set(d, unsigned char, atol(buf));
}

struct dts_field_spec dts_type_ubyte_fields[] = {
  { "string",   "string",	smacqtype_ubyte_get_string },
  { NULL,        NULL }
};

struct dts_type_info dts_type_ubyte_table = {
  size: sizeof(char),
  fromstring: parse_ubyte
};

