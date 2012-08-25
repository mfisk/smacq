#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <dts-module.h>

static int smacqtype_int64_get_string(DtsObject o, DtsObject field) {
  field->setsize(64); // Only has to hold log10(2**64)
#if __WORDSIZE==64
  field->setsize(snprintf((char*)field->getdata(), 64, "%ld", dts_data_as(o, int64_t)));
#else
  field->setsize(snprintf((char*)field->getdata(), 64, "%lld", dts_data_as(o, int64_t)));
#endif

  return 1;
}

static int smacqtype_int64_get_double(DtsObject o, DtsObject field) {
  double d = dts_data_as(o, int64_t);
  return dts_set(field, double, d);
}

static int parse_string(const char* buf, DtsObject d) {
        int64_t val;
        char * left = NULL;
        val = strtoll(buf, &left, 10);
        if (left == buf) {
                return(0);
        }
 
        return dts_set(d, int64_t, val);
}
 
static int int64_lt(void * num1, int size1, void * num2, int size2) {
        int64_t * a = (int64_t*)num1;
        int64_t * b = (int64_t*)num2;
 
        assert(size1 == sizeof(int64_t));
        assert(size2 == sizeof(int64_t));
 
        return(*a < *b);
}
 


struct dts_field_spec dts_type_int64_fields[] = {
  { "string",   "string",	smacqtype_int64_get_string },
  { "double",   "double",	smacqtype_int64_get_double },
  { NULL,        NULL }
};

struct dts_type_info dts_type_int64_table = {
        size: 8,
	fromstring: parse_string,
	lt: int64_lt
};

