#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <dts-module.h>

static int smacqtype_netlong_get_string(DtsObject o, DtsObject field) {
  field->setsize(64); // Only has to hold log10(2**32)
  field->setsize(1+snprintf((char*)field->getdata(), 64, "%u", dts_data_as(o, unsigned int)));
  return 1;
}

struct dts_field_spec dts_type_netlong_fields[] = {
  { "string",   "string",	smacqtype_netlong_get_string },
  { NULL,        NULL }
};

struct dts_type_info dts_type_netlong_table = {
        size: sizeof(unsigned long)
};

