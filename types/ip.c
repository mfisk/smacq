#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <smacq.h>

static int smacqtype_ip_get_string(const dts_object * datum, dts_object * data) {
  dts_setsize(data, INET_ADDRSTRLEN);
  return (0 != inet_ntop(AF_INET, datum->data, data->data, INET_ADDRSTRLEN));
}

static int parse_ip(char * buf,  const dts_object * d) {
  dts_setsize(d, sizeof(struct in_addr));
  return inet_pton(AF_INET, buf, d->data);
}

struct dts_field_spec dts_type_ip_fields[] = {
        { "string",    "string",	smacqtype_ip_get_string },
        { END,        NULL,     	NULL }
};

struct dts_type_info dts_type_ip_table = {
  size: 4,
  fromstring: parse_ip
};
