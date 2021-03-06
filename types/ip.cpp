#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dts-module.h>

static int smacqtype_ip_get_string(DtsObject datum, DtsObject data) {
  data->setsize(INET_ADDRSTRLEN);
  const char * r = inet_ntop(AF_INET, datum->getdata(), (char*)data->getdata(), INET_ADDRSTRLEN);
  data->setsize(strlen((char*)data->getdata()));
  return (NULL != r);
}

static int smacqtype_ip_get_nuint32(DtsObject datum, DtsObject data) {
  dts_data_as(data, uint32_t) = dts_data_as(datum, uint32_t);
  return 1;
}

static int parse_ip(const char* buf,  DtsObject d) {
  d->setsize(sizeof(struct in_addr));
  if (inet_pton(AF_INET, buf, d->getdata()) == 1) {
	return 1; /* Success */
  } else {
	/* error, try DNS */
 	struct addrinfo * resp;
	int res = getaddrinfo(buf, NULL, NULL, &resp);
	if (res == 0) {
		struct sockaddr_in * sin = (struct sockaddr_in *)resp->ai_addr;
		assert(resp->ai_family == AF_INET);
		assert(resp->ai_addr);
		memcpy(d->getdata(), &sin->sin_addr, sizeof(struct in_addr));
		freeaddrinfo(resp);
		return 1;
	} else {
		return 0;
	}
  }
}

struct dts_field_spec dts_type_ip_fields[] = {
        { "string",    "string",	smacqtype_ip_get_string },
        { "nuint32",    "nuint32",	smacqtype_ip_get_nuint32 },
        { NULL,        NULL,     	NULL }
};

struct dts_type_info dts_type_ip_table = {
  size: 4,
  fromstring: parse_ip
};
