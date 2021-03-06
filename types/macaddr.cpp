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
#include <dts-module.h>

static int smacqtype_macaddr_get_string(DtsObject o, DtsObject field) {
  unsigned char * c = (unsigned char*)o->getdata();
  field->setsize(18); 
  snprintf((char*)field->getdata(), 36, "%02x:%02x:%02x:%02x:%02x:%02x", c[0], c[1], c[2], c[3], c[4], c[5]);
  field->setsize(17); 
  return 1;
}

static int parse_macaddr(const char* buf,  DtsObject d) {
  unsigned char * c;

  d->setsize(6);
  c = d->getdata();
  sscanf(buf, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx", c+0, c+1, c+2, c+3, c+4, c+5);

  return 1;
}

struct dts_field_spec dts_type_macaddr_fields[] = {
	{ "string",   "string",	smacqtype_macaddr_get_string },
	{ NULL,        NULL }
};

struct dts_type_info dts_type_macaddr_table = {
  size: 6,
  fromstring: parse_macaddr
};
