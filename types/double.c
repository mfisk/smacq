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

static int double_lt(void * num1, int size1, void * num2, int size2) {
        double * a = num1;
        double * b = num2;

        assert(size1 == sizeof(double));
        assert(size2 == sizeof(double));

        return(*a < *b);
}

struct dts_field_spec dts_type_double_fields[] = {
  { "string",   "string",	smacqtype_double_get_string },
  { "double",   "double",	smacqtype_double_get_double },
  { END,        NULL }
};

struct dts_type_info dts_type_double_table = {
        size: sizeof(double),
  	fromstring: parse_string,
	lt: double_lt
};

