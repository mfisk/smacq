#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#include <time.h>
#include <netinet/in.h>
#include <dts-module.h>
#include "getdate_tv.h"

static int smacqtype_timeval_get_sec(DtsObject o, DtsObject field) {
  struct timeval * t = (struct timeval *)o->getdata();
  unsigned long sec = t->tv_sec;
  return dts_set(field, unsigned long, sec);
}

static int smacqtype_timeval_get_double(DtsObject o, DtsObject field) {
  struct timeval * t = (struct timeval *)o->getdata();
  double dbl = (double)t->tv_sec + 1e-6 * (double)t->tv_usec;
  return dts_set(field, double, dbl);
}

static int smacqtype_timeval_get_string(DtsObject o, DtsObject field) {
  struct timeval * t = (struct timeval *)o->getdata();
  field->setsize(64);
  snprintf((char*)field->getdata(), 64, "%lu.%06lu", (unsigned long)t->tv_sec, (unsigned long)t->tv_usec);
  return 1;
}

static int smacqtype_timeval_get_ctime(DtsObject o, DtsObject field) {
  struct timeval * t = (struct timeval *)o->getdata();
  struct tm tm;
  field->setsize(32);
  strftime((char*)field->getdata(), 32, "%T", localtime_r((time_t*)&(t->tv_sec), &tm));
  
  return 1;
}

static int smacqtype_timeval_get_date(DtsObject o, DtsObject field) {
  struct timeval * t = (struct timeval *)o->getdata();
  struct tm tm;
  field->setsize(32);
  strftime((char*)field->getdata(), 32, "%Y-%m-%d", localtime_r((time_t*)&(t->tv_sec), &tm));
  
  return 1;
}

static int parse_timeval(char * buf,  DtsObject d) {
  struct timeval tv, now;
  gettimeofday(&now, NULL);
  assert(get_date_tv(&tv, buf, &now));
  return dts_set(d, struct timeval, tv);
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

struct dts_field_spec dts_type_timeval_fields[] = {
  { "string",		"string",	smacqtype_timeval_get_string },
  { "string",		"ctime",	smacqtype_timeval_get_ctime },
  { "string",		"date",	smacqtype_timeval_get_date },
  { "double", 	"double",	smacqtype_timeval_get_double },
  { "uint32", 	"sec",	smacqtype_timeval_get_sec },
  { NULL,        NULL }
};

struct dts_type_info dts_type_timeval_table = {
  size:sizeof(struct timeval),
  fromstring:parse_timeval,
  lt:timeval_lt,
};
