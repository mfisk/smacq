#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <dts-module.h>

static int smacqtype_int_get_string(DtsObject o, DtsObject field) {
  field->setsize(64); // Only has to hold log10(2**32)
  snprintf((char*)field->getdata(), 64, "%d", dts_data_as(o, int));
  return 1;
}

static int smacqtype_int_get_hexstring(DtsObject o, DtsObject field) {
  field->setsize(64); // Only has to hold log10(2**32)
  snprintf((char*)field->getdata(), 64, "%x", dts_data_as(o, unsigned int));
  return 1;
}

static int smacqtype_int_get_double(DtsObject o, DtsObject field) {
  double d = dts_data_as(o, int);
  return dts_set(field, double, d);
}

static int parse_string(char * buf, DtsObject d) {
	int val;
	char * left = NULL;
	val = strtol(buf, &left, 10);
 	if (left == buf) {
		return(0);
	}

	return dts_set(d, int, val);
}

int int_lt(void * num1, int size1, void * num2, int size2) {
	int * a = (int*)num1;
	int * b = (int*)num2;

	assert(size1 == sizeof(int));
	assert(size2 == sizeof(int));

	return(*a < *b);
}

struct dts_field_spec dts_type_int_fields[] = {
  { "string",   "string",	smacqtype_int_get_string },
  { "string",   "hexstring",	smacqtype_int_get_hexstring },
  { "double",   "double",	smacqtype_int_get_double },
  { NULL,        NULL }
};

struct dts_type_info dts_type_int_table = {
        size: sizeof(int),
  	fromstring: parse_string,
	lt: int_lt
};

