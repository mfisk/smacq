#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/in.h>
#include <dts-module.h>

static int smacqtype_ntime_get_double(DtsObject o, DtsObject field) {
  time_t t = dts_data_as(o, time_t);
  double d = ntohl(t);
  return dts_set(field, double, d);
}

static int smacqtype_ntime_get_time(DtsObject o, DtsObject field) {
  time_t t = dts_data_as(o, time_t);
  return dts_set(field, time_t, ntohl(t));
}

static int smacqtype_ntime_get_string(DtsObject o, DtsObject field) {
  time_t t = dts_data_as(o, time_t);
  field->setsize(64);
  snprintf((char*)field->getdata(), 64, "%lu", (time_t)ntohl(t));
  return 1;
}

static int smacqtype_ntime_get_ctime(DtsObject o, DtsObject field) {
  struct tm tm;
  time_t t = ntohl(dts_data_as(o, time_t));
  field->setsize(32);
  strftime((char*)field->getdata(), 32, "%T", localtime_r(&t, &tm));
 
  return 1;
}

static int smacqtype_ntime_get_date(DtsObject o, DtsObject field) {
  struct tm tm;
  time_t t = ntohl(dts_data_as(o, time_t));
  field->setsize(32);
  strftime((char*)field->getdata(), 32, "%F", localtime_r(&t, &tm));
 
  return 1;
}
struct dts_field_spec dts_type_ntime_fields[] = {
  { "time",	"time",		smacqtype_ntime_get_time },
  { "string",	"string",	smacqtype_ntime_get_string },
  { "string",	"ctime",	smacqtype_ntime_get_ctime },
  { "string",	"date",		smacqtype_ntime_get_date },
  { "double",	"double",	smacqtype_ntime_get_double },
  { NULL,        NULL }
};

struct dts_type_info dts_type_ntime_table = {
  size:sizeof(time_t),
};
