#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "smacq.h"

static int smacqtype_int_get_string(const dts_object * o, dts_object * field) {
  dts_setsize(field, 64); // Only has to hold log10(2**32)
  snprintf(field->data, 64, "%d", dts_data_as(o, int));
  return 1;
}

static int smacqtype_int_get_hexstring(const dts_object * o, dts_object * field) {
  dts_setsize(field, 64); // Only has to hold log10(2**32)
  snprintf(field->data, 64, "%x", dts_data_as(o, unsigned int));
  return 1;
}

static int smacqtype_int_get_double(const dts_object * o, dts_object * field) {
  double d = dts_data_as(o, int);
  return dts_set(field, double, d);
}

static int parse_string(char * buf, const dts_object * d) {
	int val;
	char * left = NULL;
	val = strtol(buf, &left, 10);
 	if (left == buf) {
		return(0);
	}

	return dts_set(d, int, val);
}

int int_lt(void * num1, int size1, void * num2, int size2) {
	int * a = num1;
	int * b = num2;

	assert(size1 == sizeof(int));
	assert(size2 == sizeof(int));

	return(*a < *b);
}

struct dts_field_descriptor dts_type_int_fields[] = {
  { "string",   "string",	smacqtype_int_get_string },
  { "string",   "hexstring",	smacqtype_int_get_hexstring },
  { "double",   "double",	smacqtype_int_get_double },
  { END,        NULL }
};

struct dts_type_info dts_type_int_table = {
        size: sizeof(int),
  	fromstring: parse_string,
	lt: int_lt
};

