#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "smacq.h"

static int smacqtype_uint64_get_string(const dts_object * o, dts_object * field) {
  dts_setsize(field, 64); // Only has to hold log10(2**64)
  snprintf(field->data, 64, "%llu", dts_data_as(o, uint64_t));
  return 1;
}

static int smacqtype_uint64_get_hexstring(const dts_object * o, dts_object * field) {
  dts_setsize(field, 16); // 4 bytes -> 8 chars + 3 = 11
  snprintf(field->data, 64, "0x%llx", dts_data_as(o, uint64_t));
  return 1;
}

static int smacqtype_uint64_get_double(const dts_object * o, dts_object * field) {
  double d = dts_data_as(o, uint64_t);
  return dts_set(field, double, d);
}

static int parse_string(char * buf, const dts_object * d) {
        uint64_t val;
        char * left = NULL;
        val = strtol(buf, &left, 10);
        if (left == buf) {
                return(0);
        }
 
        return dts_set(d, uint64_t, val);
}
 
static int uint64_lt(void * num1, int size1, void * num2, int size2) {
        uint64_t * a = num1;
        uint64_t * b = num2;
 
        assert(size1 == sizeof(uint64_t));
        assert(size2 == sizeof(uint64_t));
 
        return(*a < *b);
}
 


struct dts_field_spec dts_type_uint64_fields[] = {
  { "string",   "string",	smacqtype_uint64_get_string },
  { "string",   "hexstring",	smacqtype_uint64_get_hexstring },
  { "double",   "double",	smacqtype_uint64_get_double },
  { END,        NULL }
};

struct dts_type_info dts_type_uint64_table = {
        size: 8,
	fromstring: parse_string,
	lt: uint64_lt
};

