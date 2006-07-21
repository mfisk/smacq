#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/in.h>
#include <dts-module.h>
#include <getdate_tv.h>

static int smacqtype_ntime_get_double(DtsObject o, DtsObject field) {
  uint32_t t = dts_data_as(o, uint32_t);
  double d = ntohl(t);
  return dts_set(field, double, d);
}

static int smacqtype_ntime_get_time(DtsObject o, DtsObject field) {
  uint32_t t = dts_data_as(o, uint32_t);
  return dts_set(field, uint32_t, ntohl(t));
}

static int smacqtype_ntime_get_string(DtsObject o, DtsObject field) {
  uint32_t t = dts_data_as(o, uint32_t);
  return dts_set_object_to_time_string(field, ntohl(t), 0);
}

static int smacqtype_ntime_get_ctime(DtsObject o, DtsObject field) {
  struct tm tm;
  time_t t = ntohl(dts_data_as(o, uint32_t));
  field->setsize(32);
  strftime((char*)field->getdata(), 32, "%T", localtime_r(&t, &tm));
 
  return 1;
}

static int smacqtype_ntime_get_sql(DtsObject o, DtsObject field) {
  struct tm tm;
  time_t t = ntohl(dts_data_as(o, uint32_t));
  field->setsize(32);
  strftime((char*)field->getdata(), 32, "%Y-%m-%d %T", localtime_r(&t, &tm));
 
  return 1;
}

static int smacqtype_ntime_get_date(DtsObject o, DtsObject field) {
  struct tm tm;
  time_t t = ntohl(dts_data_as(o, uint32_t));
  field->setsize(32);
  strftime((char*)field->getdata(), 32, "%F", localtime_r(&t, &tm));
 
  return 1;
}

static int parse_timeval(const char* buf,  DtsObject d) {
  struct timeval tv = {0,0};
  assert(get_date_tv(&tv, buf));
  tv.tv_sec = htonl(tv.tv_sec);
  return dts_set(d, uint32_t, tv.tv_sec);
} 

struct dts_field_spec dts_type_ntime_fields[] = {
  { "time",	"time",		smacqtype_ntime_get_time },
  { "string",	"string",	smacqtype_ntime_get_string },
  { "string",	"sql",		smacqtype_ntime_get_sql },
  { "string",	"ctime",	smacqtype_ntime_get_ctime },
  { "string",	"date",		smacqtype_ntime_get_date },
  { "double",	"double",	smacqtype_ntime_get_double },
  { NULL,        NULL }
};

struct dts_type_info dts_type_ntime_table = {
  size: sizeof(uint32_t),
  fromstring: parse_timeval
};
