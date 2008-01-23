#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <dts-module.h>

static int smacqtype_uint64_get_string(DtsObject o, DtsObject field) {
  field->setsize(64); // Only has to hold log10(2**64)
#if __WORDSIZE==64
  field->setsize(1+snprintf((char*)field->getdata(), 64, "%lu", dts_data_as(o, uint64_t)));
#else
  field->sizesize(1+snprintf((char*)field->getdata(), 64, "%llu", dts_data_as(o, uint64_t)));
#endif

  return 1;
}

static int smacqtype_uint64_get_hexstring(DtsObject o, DtsObject field) {
  field->setsize(16); // 4 bytes -> 8 chars + 3 = 11
#if __WORDSIZE==64
  field->setsize(snprintf((char*)field->getdata(), 64, "0x%lx", dts_data_as(o, uint64_t)));
#else
  field->setsize(snprintf((char*)field->getdata(), 64, "0x%llx", dts_data_as(o, uint64_t)));
#endif
  return 1;
}

static int smacqtype_uint64_get_double(DtsObject o, DtsObject field) {
  double d = dts_data_as(o, uint64_t);
  return dts_set(field, double, d);
}

static int parse_string(const char* buf, DtsObject d) {
        uint64_t val;
        char * left = NULL;
        val = strtoull(buf, &left, 10);
        if (left == buf) {
                return(0);
        }
 
        return dts_set(d, uint64_t, val);
}
 
static int uint64_lt(void * num1, int size1, void * num2, int size2) {
        uint64_t * a = (uint64_t*)num1;
        uint64_t * b = (uint64_t*)num2;
 
        assert(size1 == sizeof(uint64_t));
        assert(size2 == sizeof(uint64_t));
 
        return(*a < *b);
}
 


struct dts_field_spec dts_type_uint64_fields[] = {
  { "string",   "string",	smacqtype_uint64_get_string },
  { "string",   "hexstring",	smacqtype_uint64_get_hexstring },
  { "double",   "double",	smacqtype_uint64_get_double },
  { NULL,        NULL }
};

struct dts_type_info dts_type_uint64_table = {
        size: 8,
	fromstring: parse_string,
	lt: uint64_lt
};

