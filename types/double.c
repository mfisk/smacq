#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "smacq.h"

static int smacqtype_double_get_double(const dts_object * o, dts_object * field) {
  return dts_set(field, double, dts_data_as(o, double));
}

static int smacqtype_double_get_string(const dts_object * o, dts_object * field) {
  dts_setsize(field, 64);
  snprintf(field->data, 64, "%g", dts_data_as(o, double));
  return 1;
}

static int parse_string(char * buf,  const dts_object * d) {
  double dbl;
  char * left = NULL;
  dbl = strtod(buf, &left);
  if (left == buf) {
	return 0;
  }  
  return dts_set(d, double, dbl);
}

struct dts_field_descriptor dts_type_double_fields[] = {
  { "string",   "string",	smacqtype_double_get_string },
  { "double",   "double",	smacqtype_double_get_double },
  { END,        NULL }
};

struct dts_type_info dts_type_double_table = {
        size: sizeof(double),
  	fromstring: parse_string
};

