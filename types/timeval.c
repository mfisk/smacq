#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#include "smacq.h"

#include <netinet/in.h>

static int smacqtype_timeval_get_double(const dts_object * o, dts_object * field) {
  struct timeval * t = dts_getdata(o);
  double * dblp = malloc(sizeof(double));
  *dblp = (double)t->tv_sec + 1e-6 * (double)t->tv_usec;
  field->data = dblp;
  field->len = sizeof(double);
  field->free_data = 1;

  return 1;
}
static int smacqtype_timeval_get_string(const dts_object * o, dts_object * field) {
  struct timeval * t = dts_getdata(o);
  char buf[64]; 

  snprintf(buf, 64, "%lu.%06lu", t->tv_sec, t->tv_usec);
  field->data= strdup(buf);
  field->len= strlen(buf);
  field->free_data = 1;

  return 1;
}

static int parse_timeval(char * buf,  const dts_object * d) {
  struct timeval rv;
  double time = atol(buf);
  rv.tv_sec = floor(time);
  rv.tv_usec = (time - floor(time)) * 1e6;

  return dts_set(d, struct timeval, rv);
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

struct dts_field_descriptor dts_type_timeval_fields[] = {
  { "string",		"string",	smacqtype_timeval_get_string },
  { "double", 	"double",	smacqtype_timeval_get_double },
  { END,        NULL }
};

struct dts_type_info dts_type_timeval_table = {
  size:sizeof(struct timeval),
  fromstring:parse_timeval,
  lt:timeval_lt,
};
