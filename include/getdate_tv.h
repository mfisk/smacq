#include "config.h"
#include <ccpp.h>
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#endif
#ifndef _XOPEN_SOURCE_EXTENDED
#define _XOPEN_SOURCE_EXTENDED
#endif
#include <time.h>

BEGIN_C_DECLS
#include "strftime.h" // gnulib strftime with portable %z
#include <getdate.h>

static struct timespec now = {0, 0};

static inline bool get_date_tv (struct timeval * t, char const * s) {
	struct timespec ts;
	//ts.tv_sec = t->tv_sec;
	//ts.tv_nsec = t->tv_usec*1000;

	// Do gettime() one and only once; then cache and reuse
	if (now.tv_sec == 0) 
		gettime(&now);

	if (get_date(&ts, s, &now)) {
		t->tv_sec = ts.tv_sec;
		t->tv_usec = ts.tv_nsec / 1000;
		return true;
    	} else {
		// Try to scan as just an epoch time
		char * endp;
		double d = strtod(s, &endp);
		if (d && endp != s && endp[0] == '\0') {
			// Epochtime and nothing but epochtime
			t->tv_sec = (time_t)d;
			t->tv_usec = (time_t)((d - t->tv_sec) * 1000);

			return true;
		} else {
			fprintf(stderr, "error parsing time string %s", s);
			return false;
		}
    	}
}

END_C_DECLS	

static inline bool dts_set_object_to_time_string(DtsObject field, time_t seconds, int usec) {
  field->setsize(64);
  char * result = (char*)field->getdata();

  struct tm time_tm;
  localtime_r(&seconds, &time_tm);

  result += nstrftime(result, 64, "%Y-%m-%dT%H:%M:%S", &time_tm, 0, 0);
  if (usec) {
        result += sprintf(result, ".%06d", usec);
  }
  result += nstrftime(result, 64, "%:z", &time_tm, 0, 0);
  field->setsize(result - (char*)field->getdata());
  return true;
}


