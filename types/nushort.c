#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include "smacq.h"

static int flowtype_nushort_get_string(void * data, int dlen, void ** transform, int * tlen) {
  char buf[64]; // Only has to hold log10(2**32)

  assert(dlen==sizeof(unsigned short));

  snprintf(buf, 64, "%hu", ntohs(*(unsigned short*)data));
  *transform = strdup(buf);
  *tlen = strlen(data);

  return 1;
}

static int parse_nushort(char * buf, void ** resp, int * reslen) {
  ushort * us = g_new(ushort, 1);
  *us = htons(atol(buf));

  *resp = us;
  *reslen = sizeof(unsigned short);

  return 1;
}

int nushort_lt(void * num1, int size1, void * num2, int size2) {
	ushort a, b;

	assert(size1 == sizeof(ushort));
	assert(size2 == sizeof(ushort));

	a = ntohs(*(ushort*)num1);
	b = ntohs(*(ushort*)num2);

	return(a < b);
}
      
struct dts_transform_descriptor dts_type_nushort_transforms[] = {
	{ "string",   flowtype_nushort_get_string },
        { END,        NULL }
};

struct dts_type_info dts_type_nushort_table = {
  size: sizeof(unsigned short),
  fromstring: parse_nushort,
  lt: nushort_lt
};
