#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <smacq.h>
#include <netinet/in.h>

static int smacqtype_ntime_get_time(const dts_object * o, dts_object * field) {
  time_t t = dts_data_as(o, time_t);
  return dts_set(field, time_t, ntohl(t));
}

static int smacqtype_ntime_get_string(const dts_object * o, dts_object * field) {
  time_t t = dts_data_as(o, time_t);
  dts_setsize(field, 64);
  snprintf(field->data, 64, "%lu", (time_t)ntohl(t));
  return 1;
}

static int smacqtype_ntime_get_ctime(const dts_object * o, dts_object * field) {
  struct tm tm;
  time_t t = ntohl(dts_data_as(o, time_t));
  dts_setsize(field, 32);
  strftime(field->data, 32, "%T", localtime_r(&t, &tm));
 
  return 1;
}

static int smacqtype_ntime_get_date(const dts_object * o, dts_object * field) {
  struct tm tm;
  time_t t = ntohl(dts_data_as(o, time_t));
  dts_setsize(field, 32);
  strftime(field->data, 32, "%F", localtime_r(&t, &tm));
 
  return 1;
}

                                                                                                                          
struct dts_field_spec dts_type_ntime_fields[] = {
  { "time",	"time",		smacqtype_ntime_get_time },
  { "string",	"string",	smacqtype_ntime_get_string },
  { "string",	"ctime",	smacqtype_ntime_get_ctime },
  { "string",	"date",		smacqtype_ntime_get_date },
  { END,        NULL }
};

struct dts_type_info dts_type_ntime_table = {
  size:sizeof(time_t),
};
