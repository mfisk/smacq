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

// 64-bit Linux (probably others) uses 64-bit values for struct timeval.
// This is a 32-bit timeval regardless of platform, so we use our own struct.

struct timeval_32 {
	uint32_t tv_sec;
	uint32_t tv_usec;
};

static int smacqtype_timeval_get_sec(DtsObject o, DtsObject field) {
  struct timeval_32 * t = (struct timeval_32 *)o->getdata();
  unsigned long sec = t->tv_sec;
  return dts_set(field, unsigned long, sec);
}

static int smacqtype_timeval_get_double(DtsObject o, DtsObject field) {
  struct timeval_32 * t = (struct timeval_32 *)o->getdata();
  double dbl = (double)t->tv_sec + 1e-6 * (double)t->tv_usec;
  return dts_set(field, double, dbl);
}

static int smacqtype_timeval_get_string(DtsObject o, DtsObject field) {
  struct timeval_32 * t = (struct timeval_32 *)o->getdata();
  field->setsize(64);
  snprintf((char*)field->getdata(), 64, "%lu.%06lu", (unsigned long)t->tv_sec, (unsigned long)t->tv_usec);
  return 1;
}

static int smacqtype_timeval_get_sql(DtsObject o, DtsObject field) {
  struct timeval_32 * t = (struct timeval_32 *)o->getdata();
  struct tm tm;
  field->setsize(32);
  strftime((char*)field->getdata(), 32, "%Y-%m-%d %T", localtime_r((time_t*)&(t->tv_sec), &tm));
  
  return 1;
}

static int smacqtype_timeval_get_ctime(DtsObject o, DtsObject field) {
  struct timeval_32 * t = (struct timeval_32 *)o->getdata();
  struct tm tm;
  field->setsize(32);
  strftime((char*)field->getdata(), 32, "%T", localtime_r((time_t*)&(t->tv_sec), &tm));
  
  return 1;
}

static int smacqtype_timeval_get_date(DtsObject o, DtsObject field) {
  struct timeval_32 * t = (struct timeval_32 *)o->getdata();
  struct tm tm;
  field->setsize(32);
  strftime((char*)field->getdata(), 32, "%Y-%m-%d", localtime_r((time_t*)&(t->tv_sec), &tm));
  
  return 1;
}

static int parse_timeval(const char* buf,  DtsObject d) {
  struct timeval tv;
  assert(get_date_tv(&tv, buf));
  if (sizeof(struct timeval) != sizeof(struct timeval_32)) {
  	struct timeval_32 tv32;
  	tv32.tv_sec = tv.tv_sec;
  	tv32.tv_usec = tv.tv_usec;
  	return dts_set(d, struct timeval_32, tv32);
  } else {
  	return dts_set(d, struct timeval, tv);
  }
}

int timeval_ge(struct timeval_32 x, struct timeval_32 y) {
  if (x.tv_sec > y.tv_sec) return 1;
  if ((x.tv_sec == y.tv_sec) && (x.tv_usec >= y.tv_usec)) return 1;
  return 0;
}

static int timeval_lt(void * p1, int len1, void * p2, int len2) {
  assert(len1 == sizeof(struct timeval_32));
  assert(len2 == sizeof(struct timeval_32));

  return(!timeval_ge(*(struct timeval_32*)p1, *(struct timeval_32*)p2));
}

struct dts_field_spec dts_type_timeval_fields[] = {
  { "string",		"string",	smacqtype_timeval_get_string },
  { "string",		"ctime",	smacqtype_timeval_get_ctime },
  { "string",		"date",	smacqtype_timeval_get_date },
  { "string",		"sql",	smacqtype_timeval_get_sql },
  { "double", 	"double",	smacqtype_timeval_get_double },
  { "uint32", 	"sec",	smacqtype_timeval_get_sec },
  { "time", 	"time",	smacqtype_timeval_get_sec },
  { NULL,        NULL }
};

struct dts_type_info dts_type_timeval_table = {
  size:sizeof(struct timeval_32), 
  fromstring:parse_timeval,
  lt:timeval_lt,
};
