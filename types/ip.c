#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <smacq.h>

static int smacqtype_ip_get_string(const dts_object * datum, dts_object * data) {
  dts_setsize(data, INET_ADDRSTRLEN);
  return (0 != inet_ntop(AF_INET, datum->data, data->data, INET_ADDRSTRLEN));
}

static int parse_ip(char * buf,  const dts_object * d) {
  dts_setsize(d, sizeof(struct in_addr));
  if (inet_pton(AF_INET, buf, d->data) == 1) {
	fprintf(stderr, "pton ok on %s\n", buf);
	return 1;
  } else {
	/* error, try DNS */
 	struct addrinfo * resp;
	int res = getaddrinfo(buf, NULL, NULL, &resp);
	if (res == 0) {
		struct sockaddr_in * sin = (struct sockaddr_in *)resp->ai_addr;
		assert(resp->ai_family == AF_INET);
		assert(resp->ai_addr);
		memcpy(d->data, &sin->sin_addr, sizeof(struct in_addr));
		freeaddrinfo(resp);
		return 1;
	} else {
		return 0;
	}
  }
}

struct dts_field_spec dts_type_ip_fields[] = {
        { "string",    "string",	smacqtype_ip_get_string },
        { END,        NULL,     	NULL }
};

struct dts_type_info dts_type_ip_table = {
  size: 4,
  fromstring: parse_ip
};
