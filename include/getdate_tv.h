#include <ccpp.h>
BEGIN_C_DECLS
#include <getdate.h>

static inline bool get_date_tv (struct timeval * t, char const * s, struct timeval const * now) {
	struct timespec ts, tsnow;
	ts.tv_sec = t->tv_sec;
	ts.tv_nsec = t->tv_usec*1000;
	tsnow.tv_sec = now->tv_sec;
	tsnow.tv_nsec = now->tv_usec*1000;
	if (get_date(&ts, s, &tsnow)) {
		t->tv_sec = ts.tv_sec;
		t->tv_usec = ts.tv_nsec / 1000;
		return true;
    } else {
		return false;
	}
}

END_C_DECLS	

