#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <netdb.h>
#include "smacq.h"

static int smacqtype_ipproto_get_string(const dts_object * obj, dts_object * field) {
  struct protoent * p = getprotobynumber((dts_data_as(obj, unsigned char)));
  char * buf;
  char scratch[64]; // Only has to hold log10(2**32)

  if (p) {
	buf = p->p_name;
  } else {
	buf = scratch;
  	snprintf(buf, 64, "%hu", dts_data_as(obj, unsigned char)); 
  }

  field->data = strdup(buf);
  field->len = strlen(buf);
  field->free_data = 1;

  return 1;
}

static int parse_ipproto(char * buf, void ** resp, int * reslen) {
  unsigned char * us = malloc(sizeof(unsigned char));
  struct protoent * p = getprotobyname(buf);
  if (p) {
	  *us = p->p_proto;
  } else {
	  *us = atol(buf);
  }

  *resp = us;
  *reslen = sizeof(unsigned char);

  return 1;
}

static int ipproto_lt(void * num1, int size1, void * num2, int size2) {
	unsigned char a, b;

	assert(size1 == sizeof(unsigned char));
	assert(size2 == sizeof(unsigned char));

	a = *(unsigned char*)num1;
	b = *(unsigned char*)num2;

	return(a < b);
}

struct dts_field_descriptor dts_type_ipproto_fields[] = {
	{ "string",   "string",	smacqtype_ipproto_get_string },
        { END,        NULL }
};

struct dts_type_info dts_type_ipproto_table = {
  size: sizeof(unsigned char),
  fromstring: parse_ipproto,
  lt: ipproto_lt
};
