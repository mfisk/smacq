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

static int parse_ip(char * buf,  const dts_object * d) {
  struct in_addr a;
  if (!inet_aton(buf, &a)) {
    return 0;
  } else {
    return dts_set(d, struct in_addr, a);
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
