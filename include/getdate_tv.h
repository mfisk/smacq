#include <ccpp.h>
BEGIN_C_DECLS
#include <getdate.h>

static struct timespec now = {0, 0};

static inline bool get_date_tv (struct timeval * t, char const * s) {
	struct timespec ts;
	ts.tv_sec = t->tv_sec;
	ts.tv_nsec = t->tv_usec*1000;

	// Do gettime() one and only once; then cache and reuse
	if (now.tv_sec == 0) 
		assert(gettime(&now) == 0);

	if (get_date(&ts, s, &now)) {
		t->tv_sec = ts.tv_sec;
		t->tv_usec = ts.tv_nsec / 1000;
		return true;
    	} else {
		return false;
    	}
}

END_C_DECLS	

