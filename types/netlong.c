#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include "smacq.h"

static int smacqtype_netlong_get_string(const dts_object * o, dts_object * field) {
  char buf[64]; // Only has to hold log10(2**32)

  snprintf(buf, 64, "%u", dts_data_as(o, unsigned int));
  field->data= strdup(buf);
  field->len= strlen(buf);
  field->free_data = 1;

  return 1;
}

struct dts_field_descriptor dts_type_netlong_fields[] = {
  { "string",   "string",	smacqtype_netlong_get_string },
  { END,        NULL }
};

struct dts_type_info dts_type_netlong_table = {
        size: sizeof(unsigned long)
};

