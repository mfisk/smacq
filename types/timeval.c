#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#include "smacq.h"

#include <netinet/in.h>

static int flowtype_timeval_get_double(void * data, int dlen, void ** transform, int * tlen) {
  struct timeval * t = data;
  double * dblp = malloc(sizeof(double));
  *dblp = (double)t->tv_sec + 1e-6 * (double)t->tv_usec;
  *transform = dblp;
  *tlen = sizeof(double);

  return 1;
}
static int flowtype_timeval_get_string(void * data, int dlen, void ** transform, int * tlen) {
  struct timeval * t = data;
  char buf[64]; 

  assert(dlen=sizeof(struct timeval));

  snprintf(buf, 64, "%lu.%06lu", t->tv_sec, t->tv_usec);
  *transform = strdup(buf);
  *tlen = strlen(data);

  return 1;
}

static int parse_timeval(char * buf, void ** resp, int * reslen) {
  struct timeval * rv = g_new(struct timeval, 1);
  double time = atol(buf);
  rv->tv_sec = floor(time);
  rv->tv_usec = (time - floor(time)) * 1e6;

  *resp = rv;
  *reslen = sizeof(struct timeval);

  return 1;
}

int timeval_ge(struct timeval x, struct timeval y) {
  if (x.tv_sec > y.tv_sec) return 1;
  if ((x.tv_sec == y.tv_sec) && (x.tv_usec >= y.tv_usec)) return 1;
  return 0;
}

static int timeval_lt(void * p1, int len1, void * p2, int len2) {
  assert(len1 == sizeof(struct timeval));
  assert(len2 == sizeof(struct timeval));

  return(!timeval_ge(*(struct timeval*)p1, *(struct timeval*)p2));
}

struct dts_transform_descriptor dts_type_timeval_transforms[] = {
	{ "string",   	flowtype_timeval_get_string },
	{ "double",   	flowtype_timeval_get_double },
        { END,        NULL }
};

struct dts_type_info dts_type_timeval_table = {
  size:sizeof(struct timeval),
  fromstring:parse_timeval,
  lt:timeval_lt,
};
