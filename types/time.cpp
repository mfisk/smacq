#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <dts-module.h>

static int smacqtype_time_get_double(DtsObject o, DtsObject field) {
  time_t t = dts_data_as(o, time_t);
  double dbl = (double)t;
  return dts_set(field, double, dbl);
}

static int smacqtype_time_get_string(DtsObject o, DtsObject field) {
  time_t t = dts_data_as(o, time_t);
  field->setsize(64);
  snprintf((char*)field->getdata(), 64, "%lu", (time_t)t);
  return 1;
}

static int smacqtype_time_get_ctime(DtsObject o, DtsObject field) {
  struct tm tm;
  time_t t = dts_data_as(o, time_t);
  field->setsize(32);
  strftime((char*)field->getdata(), 32, "%T", localtime_r(&t, &tm));
  
  return 1;
}

static int time_lt(void * p1, int len1, void * p2, int len2) {
  assert(len1 == sizeof(unsigned long));
  assert(len2 == sizeof(unsigned long));

  return((*(unsigned long*)p1 <  *(unsigned long*)p2));
}

struct dts_field_spec dts_type_time_fields[] = {
  { "string",	"string",	smacqtype_time_get_string },
  { "string",	"ctime",	smacqtype_time_get_ctime },
  { "double", 	"double",	smacqtype_time_get_double },
  { NULL,        NULL }
};

struct dts_type_info dts_type_time_table = {
  size:sizeof(time_t),
  fromstring: NULL,
  lt:time_lt,
};
