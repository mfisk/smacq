#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <dts-module.h>

static int smacqtype_ushort_get_uint32(DtsObject o, DtsObject field) {
  return dts_set(field, unsigned int, dts_data_as(o, ushort));
}

static int smacqtype_ushort_get_double(DtsObject o, DtsObject field) {
  return dts_set(field, double, dts_data_as(o, ushort));
}

static int smacqtype_ushort_get_string(DtsObject o, DtsObject field) {
  field->setsize(64); // Only has to hold log10(2**32)
  snprintf((char*)field->getdata(), 64, "%hu", dts_data_as(o, unsigned short));
  return 1;
}

static int parse_ushort(char * buf,  DtsObject d) {
  return dts_set(d, ushort, atol(buf));
}

struct dts_field_spec dts_type_ushort_fields[] = {
  { "string",   "string",	smacqtype_ushort_get_string },
  { "uint32",   "uint32",	smacqtype_ushort_get_uint32 },
  { "double",   "double",	smacqtype_ushort_get_double },
  { NULL,        NULL }
};

struct dts_type_info dts_type_ushort_table = {
  size: sizeof(unsigned short),
  fromstring: parse_ushort


};
