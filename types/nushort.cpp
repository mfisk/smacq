#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <dts-module.h>

static int smacqtype_nushort_get_string(DtsObject obj, DtsObject field) {
  field->setsize(64); // Only has to hold log10(2**32)
  snprintf((char*)field->getdata(), 64, "%hu", ntohs(dts_data_as(obj, unsigned short)));
  return 1;
}

static int smacqtype_nushort_get_uint32(DtsObject o, DtsObject field) {
	dts_set(field, uint, ntohs(dts_data_as(o, ushort)));
	return 1;
}

static int smacqtype_nushort_get_double(DtsObject o, DtsObject field) {
	dts_set(field, double, ntohs(dts_data_as(o, ushort)));
	return 1;
}

static int parse_nushort(char * buf,  DtsObject d) {
  return dts_set(d, ushort, htons(atol(buf)));
}

int nushort_lt(void * num1, int size1, void * num2, int size2) {
	ushort a, b;

	assert(size1 == sizeof(ushort));
	assert(size2 == sizeof(ushort));

	a = ntohs(*(ushort*)num1);
	b = ntohs(*(ushort*)num2);

	return(a < b);
}

struct dts_field_spec dts_type_nushort_fields[] = {
	{ "string",   "string",	smacqtype_nushort_get_string },
	{ "uint32",   "uint32",	smacqtype_nushort_get_uint32 },
	{ "double",   "double",	smacqtype_nushort_get_double },
        { NULL,        NULL }
};

struct dts_type_info dts_type_nushort_table = {
  size: sizeof(unsigned short),
  fromstring: parse_nushort,
  lt: nushort_lt
};
