#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <dts-module.h>

static int smacqtype_nuint32_get_string(DtsObject * o, DtsObject * field) {
  field->setsize(64); // Only has to hold log10(2**32)
  snprintf((char*)field->getdata(), 64, "%u", ntohl(dts_data_as(o, unsigned int)));
  return 1;
}

static int smacqtype_nuint32_get_double(DtsObject * o, DtsObject * field) {
  double d = ntohl(dts_data_as(o, unsigned long));
  dts_data_as(field, double) = d;
  return 1;
}

static int parse_nuint32(char * buf,  DtsObject * d) {
  return dts_set(d, unsigned long, ntohl(atol(buf)));
}

static int nuint32_lt(void * num1, int size1, void * num2, int size2) {
  unsigned long * a = (unsigned long*)num1;
  unsigned long * b = (unsigned long*)num2;
                                                                                                                                                        
  assert(size1 == sizeof(unsigned long));
  assert(size2 == sizeof(unsigned long));
                                                                                                                                                        
  return(ntohl(*a) < ntohl(*b));
}
                                                                                                                                                        
struct dts_field_spec dts_type_nuint32_fields[] = {
  { "string",   "string",	smacqtype_nuint32_get_string },
  { "double",   "double",	smacqtype_nuint32_get_double },
  { NULL,        NULL }
};

struct dts_type_info dts_type_nuint32_table = {
  size: sizeof(unsigned int),
  fromstring:parse_nuint32,
  lt: nuint32_lt
};

