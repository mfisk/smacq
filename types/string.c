#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "smacq.h"

static int smacqtype_string_get_string(void * data, int dlen, void ** transform, int * tlen) {
  *transform = strdup(data);
  *tlen = dlen;

  return 1;
}

static int parse_string(char * buf, void ** resp, int * reslen) {
  *resp = buf;
  *reslen = strlen(buf);
    
  return 1;
}

struct dts_transform_descriptor dts_type_string_transforms[] = {
	{ "string",   smacqtype_string_get_string },
        { END,        NULL }
};

struct dts_type_info dts_type_string_table = {
  size: -1,
  fromstring: parse_string
};

