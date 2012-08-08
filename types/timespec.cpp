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

struct timespec_32 {
	uint32_t tv_sec;
	uint32_t tv_nsec;
};

/* Note that string handling will round to/from nearest us rather than ns */

static int smacqtype_timespec_get_sec(DtsObject o, DtsObject field) {
  struct timespec_32 * t = (struct timespec_32 *)o->getdata();
  uint32_t sec = t->tv_sec;
  return dts_set(field, uint32_t, sec);
}

static int smacqtype_timespec_get_double(DtsObject o, DtsObject field) {
  struct timespec_32 * t = (struct timespec_32 *)o->getdata();
  double dbl = (double)t->tv_sec + 1e-9 * (double)t->tv_nsec;
  return dts_set(field, double, dbl);
}

static int smacqtype_timespec_get_string(DtsObject o, DtsObject field) {
  struct timespec_32 * t = (struct timespec_32 *)o->getdata();
  return dts_set_object_to_time_string(field, t->tv_sec, t->tv_nsec/1000);
}

static int smacqtype_timespec_get_sql(DtsObject o, DtsObject field) {
  struct timespec_32 * t = (struct timespec_32 *)o->getdata();
  struct tm tm;
  field->setsize(32);
  time_t s = t->tv_sec;
  field->setsize(strftime((char*)field->getdata(), 32, "%Y-%m-%d %T", localtime_r(&s, &tm)));
  
  return 1;
}

static int smacqtype_timespec_get_ctime(DtsObject o, DtsObject field) {
  struct timespec_32 * t = (struct timespec_32 *)o->getdata();
  struct tm tm;
  field->setsize(32);
  time_t s = t->tv_sec;
  field->setsize(strftime((char*)field->getdata(), 32, "%T", localtime_r(&s, &tm)));
  
  return 1;
}

static int smacqtype_timespec_get_date(DtsObject o, DtsObject field) {
  struct timespec_32 * t = (struct timespec_32 *)o->getdata();
  struct tm tm;
  field->setsize(32);
  time_t s = t->tv_sec;
  field->setsize(strftime((char*)field->getdata(), 32, "%Y-%m-%d", localtime_r(&s, &tm)));
  
  return 1;
}

static int parse_timespec(const char* buf,  DtsObject d) {
  struct timeval tv = {0,0};
  struct timespec_32 ts32;
  assert(get_date_tv(&tv, buf));
  ts32.tv_sec = tv.tv_sec;
  ts32.tv_nsec = tv.tv_usec * 1000;
  return dts_set(d, struct timespec_32, ts32);
}

int timespec_ge(struct timespec_32 x, struct timespec_32 y) {
  if (x.tv_sec > y.tv_sec) return 1;
  if ((x.tv_sec == y.tv_sec) && (x.tv_nsec >= y.tv_nsec)) return 1;
  return 0;
}

static int timespec_lt(void * p1, int len1, void * p2, int len2) {
  assert(len1 == sizeof(struct timespec_32));
  assert(len2 == sizeof(struct timespec_32));

  return(!timespec_ge(*(struct timespec_32*)p1, *(struct timespec_32*)p2));
}

struct dts_field_spec dts_type_timespec_fields[] = {
  { "string",		"string",	smacqtype_timespec_get_string },
  { "string",		"ctime",	smacqtype_timespec_get_ctime },
  { "string",		"date",	smacqtype_timespec_get_date },
  { "string",		"sql",	smacqtype_timespec_get_sql },
  { "double", 	"double",	smacqtype_timespec_get_double },
  { "uint32", 	"uint32",	smacqtype_timespec_get_sec },
  { "uint32", 	"sec",	smacqtype_timespec_get_sec },
  { "time", 	"time",	smacqtype_timespec_get_sec },
  { NULL,        NULL }
};

struct dts_type_info dts_type_timespec_table = {
  size:sizeof(struct timespec_32), 
  fromstring:parse_timespec,
  lt:timespec_lt,
};
