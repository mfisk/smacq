#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <dts-module.h>

static int smacqtype_uint32_get_string(DtsObject o, DtsObject field) {
  field->setsize(64); // Only has to hold log10(2**32)
  snprintf(field->getdata(), 64, "%u", dts_data_as(o, unsigned int));
  return 1;
}

static int smacqtype_uint32_get_hexstring(DtsObject o, DtsObject field) {
  field->setsize(16); // 4 bytes -> 8 chars + 3 = 11
  snprintf(field->getdata(), 64, "0x%x", dts_data_as(o, unsigned int));
  return 1;
}

static int smacqtype_uint32_get_double(DtsObject o, DtsObject field) {
  unsigned long l = dts_data_as(o, unsigned long);
  double d = l;
  return dts_set(field, double, d);
}

static int parse_string(char * buf, DtsObject d) {
        unsigned long val;
        char * left = NULL;
        val = strtoul(buf, &left, 10);
        if (left == buf) {
                return(0);
        }
 
        return dts_set(d, unsigned long, val);
}
 
static int uint32_lt(void * num1, int size1, void * num2, int size2) {
        unsigned long * a = num1;
        unsigned long * b = num2;
 
        assert(size1 == sizeof(unsigned long));
        assert(size2 == sizeof(unsigned long));
 
        return(*a < *b);
}
 


struct dts_field_spec dts_type_uint32_fields[] = {
  { "string",   "string",	smacqtype_uint32_get_string },
  { "string",   "hexstring",	smacqtype_uint32_get_hexstring },
  { "double",   "double",	smacqtype_uint32_get_double },
  { NULL,        NULL }
};

struct dts_type_info dts_type_uint32_table = {
        size: sizeof(unsigned int),
	fromstring: parse_string,
	lt: uint32_lt
};

