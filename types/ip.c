#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <smacq.h>

static int smacqtype_ip_get_string(const dts_object * datum, dts_object * data) {
  data->len = 16;
  data->free_data = 1;
  data->data = malloc(16);
  strncpy(data->data, inet_ntoa(dts_data_as(datum, struct in_addr)), 16);

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

struct dts_field_descriptor dts_type_ip_fields[] = {
        { "string",    "string",	smacqtype_ip_get_string },
        { END,        NULL,     	NULL }
};

struct dts_type_info dts_type_ip_table = {
  size: 4,
  fromstring: parse_ip
};
