#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <smacq.h>

static int smacqtype_ip_get_string(void * data, int dlen, void ** transform, int * tlen) {
  assert(dlen==4);
  *transform = malloc(16);
  strncpy(*transform, inet_ntoa(*(struct in_addr*)data), 16);
  *tlen = 16;

  return 1;
}

static int parse_ip(char * buf, void ** resp, int * reslen) {
  struct in_addr * a = g_new(struct in_addr, 1);
  if (!inet_aton(buf, a)) {
    perror(buf);
    free(a);
    return 0;
  } else {
    *resp = a;
    *reslen = sizeof(struct in_addr);
    
    return 1;
  }
}

struct dts_transform_descriptor dts_type_ip_transforms[] = {
	{ "string",   smacqtype_ip_get_string },
        { END,        NULL }
};

struct dts_type_info dts_type_ip_table = {
  size: 4,
  fromstring: parse_ip
};
