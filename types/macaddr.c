#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include "smacq.h"

static int flowtype_macaddr_get_string(void * data, int dlen, void ** transform, int * tlen) {
  char buf[36]; 
  unsigned char * c = data;

  assert(dlen==6);
  snprintf(buf, 36, "%02x:%02x:%02x:%02x:%02x:%02x", c[0], c[1], c[2], c[3], c[4], c[5]);

  *transform = strdup(buf);
  *tlen = strlen(buf);

  return 1;
}

static int parse_macaddr(char * buf, void ** resp, int * reslen) {
  char * c = g_new(unsigned char, 6);
  sscanf(buf, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx", c+0, c+1, c+2, c+3, c+4, c+5);

  *resp = c;
  *reslen = 6;

  return 1;
}

struct dts_transform_descriptor dts_type_macaddr_transforms[] = {
	{ "string",   flowtype_macaddr_get_string },
	{ END,        NULL }
};

struct dts_type_info dts_type_macaddr_table = {
  size: 6,
  fromstring: parse_macaddr
};
