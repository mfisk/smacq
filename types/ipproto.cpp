#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <netdb.h>
#include <dts-module.h>

static int smacqtype_ipproto_get_double(DtsObject obj, DtsObject field) {
  return dts_set(field, double, dts_data_as(obj, unsigned char));
}

static int smacqtype_ipproto_get_string(DtsObject obj, DtsObject field) {
  uint8_t pnum = dts_data_as(obj, uint8_t);
  struct protoent * p = getprotobynumber(pnum);

  if (p) {
	field->setsize(strlen(p->p_name));
	memcpy((char*)field->getdata(), p->p_name, field->getsize());
  } else {
	field->setsize(64);
  	field->setsize(snprintf((char*)field->getdata(), 64, "%hu", dts_data_as(obj, unsigned char)));
  }

  return 1;
}

static int parse_ipproto(const char * buf,  DtsObject d) {
  unsigned char us;
  struct protoent * p = getprotobyname(buf);
  if (p) {
	  us = p->p_proto;
  } else {
	  us = atol(buf);
  }

  return dts_set(d, unsigned char, us);
}

static int ipproto_lt(void * num1, int size1, void * num2, int size2) {
	unsigned char a, b;

	assert(size1 == sizeof(unsigned char));
	assert(size2 == sizeof(unsigned char));

	a = *(unsigned char*)num1;
	b = *(unsigned char*)num2;

	return(a < b);
}

struct dts_field_spec dts_type_ipproto_fields[] = {
	{ "string",   "string",	smacqtype_ipproto_get_string },
	{ "double",   "double",	smacqtype_ipproto_get_double },
        { NULL,        NULL }
};

struct dts_type_info dts_type_ipproto_table = {
  size: sizeof(unsigned char),
  fromstring: parse_ipproto,
  lt: ipproto_lt,
};
