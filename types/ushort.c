#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "smacq.h"

static int smacqtype_ushort_get_string(void * data, int dlen, void ** transform, int * tlen) {
  char buf[64]; // Only has to hold log10(2**32)

  assert(dlen==sizeof(unsigned short));

  snprintf(buf, 64, "%hu", *(unsigned short*)data);
  *transform = strdup(buf);
  *tlen = strlen(data);

  return 1;
}

static int parse_ushort(char * buf, void ** resp, int * reslen) {
  ushort * us = g_new(ushort, 1);
  *us = atol(buf);

  *resp = us;
  *reslen = sizeof(unsigned short);

  return 1;
}

struct dts_transform_descriptor dts_type_ushort_transforms[] = {
  { "string",   smacqtype_ushort_get_string },
  { END,        NULL }
};

struct dts_type_info dts_type_ushort_table = {
  size: sizeof(unsigned short),
  fromstring: parse_ushort


};
