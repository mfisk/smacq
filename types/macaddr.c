#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#ifndef linux 
#include <netinet/in_systm.h>
#endif
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <arpa/inet.h>
#include "smacq.h"

static int smacqtype_macaddr_get_string(const dts_object * o, dts_object * field) {
  char buf[36]; 
  unsigned char * c = (unsigned char*)o->data;

  snprintf(buf, 36, "%02x:%02x:%02x:%02x:%02x:%02x", c[0], c[1], c[2], c[3], c[4], c[5]);

  field->data= strdup(buf);
  field->len= strlen(buf);
  field->free_data = 1;

  return 1;
}

static int parse_macaddr(char * buf,  const dts_object * d) {
  unsigned char * c;

  dts_setsize(d, 6);
  c = d->data;
  sscanf(buf, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx", c+0, c+1, c+2, c+3, c+4, c+5);

  return 1;
}

struct dts_field_descriptor dts_type_macaddr_fields[] = {
	{ "string",   "string",	smacqtype_macaddr_get_string },
	{ END,        NULL }
};

struct dts_type_info dts_type_macaddr_table = {
  size: 6,
  fromstring: parse_macaddr
};
