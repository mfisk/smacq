#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <dts-module.h>

static int smacqtype_uint32_get_string(DtsObject o, DtsObject field) {
  field->setsize(64); // Only has to hold log10(2**32)
  field->setsize(snprintf((char*)field->getdata(), 64, "%u", dts_data_as(o, unsigned int)));
  return 1;
}

static int smacqtype_uint32_get_hexstring(DtsObject o, DtsObject field) {
  field->setsize(16); // 4 bytes -> 8 chars + 3 = 11
  field->setsize(snprintf((char*)field->getdata(), 64, "0x%x", dts_data_as(o, unsigned int)));
  return 1;
}

static int smacqtype_uint32_get_double(DtsObject o, DtsObject field) {
  uint32_t l = dts_data_as(o, uint32_t);
  double d = l;
  return dts_set(field, double, d);
}

static int parse_string(const char* buf, DtsObject d) {
        uint32_t val;
        char * left = NULL;
        val = strtoul(buf, &left, 10);
        if (left == buf) {
                return(0);
        }
 
        return dts_set(d, uint32_t, val);
}
 
static int uint32_lt(void * num1, int size1, void * num2, int size2) {
        uint32_t * a = (uint32_t *)num1;
        uint32_t * b = (uint32_t *)num2;
 
        assert(size1 == sizeof(uint32_t));
        assert(size2 == sizeof(uint32_t));
 
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

