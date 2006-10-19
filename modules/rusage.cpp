#include <stdlib.h>
#include <assert.h>
#include <SmacqModule.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

SMACQ_MODULE(rusage,
  PROTO_CTOR(rusage);
  PROTO_CONSUME();

  int longtype;

  int rss;
  DtsField rss_field;

  int idrss;
  DtsField idrss_field;
);

#ifdef linux
int linux_getrusage(int who, struct rusage * usage) {
  int r = getrusage(who, usage);
  if (r) return r;

  if (who == RUSAGE_SELF) {
	  FILE * proc = fopen("/proc/self/stat","r");
	  int pid, ppid, pgrp, session, tty, tpgid, utime, stime, cutime,
	  	cstime, counter, priority, starttime, signal, blocked,
		sigignore, sigcatch;
	  unsigned int flags, minflt, cminflt, majflt, cmajflt, timeout, 
	  	itrealvalue, vsize, rss, rlim, startcode, endcode, 
		startstack, kstkesp, kstkeip, wchan;
	  char comm[256];
	  char state;

	  assert(proc);

	  fscanf(proc, "%d %s %c %d %d %d %d %d %u %u %u %u %u %d %d %d %d %d %d %u %u %d %u %u %u %u %u %u %u %u %d %d %d %d %u",
			  &pid, comm, &state, &ppid, &pgrp, &session, &tty, &tpgid,
			  &flags, &minflt, &cminflt, &majflt, &cmajflt, &utime,
			  &stime, &cutime, &cstime, &counter, &priority, &timeout,
			  &itrealvalue, &starttime, &vsize, &rss, &rlim, &startcode,
			  &endcode, &startstack, &kstkesp, &kstkeip, &signal,
			  &blocked, &sigignore, &sigcatch, &wchan);
	  usage->ru_maxrss = rss;
	  fclose(proc);
  }

  return r;
}
#define getrusage(who,s) linux_getrusage(who,s)

#endif

static struct smacq_options options[] = {
  {"d", {boolean_t:0}, "Integral unshared data size (not available on Linux)", SMACQ_OPT_TYPE_BOOLEAN},
  {"r", {boolean_t:1}, "Resident set size (current on Linux, MAX otherwise)", SMACQ_OPT_TYPE_BOOLEAN},
  END_SMACQ_OPTIONS
};

smacq_result rusageModule::consume(DtsObject datum, int & outchan) {
  struct rusage rusage;
  getrusage(RUSAGE_SELF, &rusage);

  if (idrss) {
    DtsObject msgdata = dts->construct(longtype, &rusage.ru_idrss);
    datum->attach_field(idrss_field, msgdata); 
  }
  if (rss) {
    DtsObject msgdata = dts->construct(longtype, &rusage.ru_maxrss);
    datum->attach_field(rss_field, msgdata); 
  }
 
  return SMACQ_PASS;
}

rusageModule::rusageModule(smacq_init * context) : SmacqModule(context) {
  int argc = 0;
  char ** argv;
  smacq_opt idrsso, rsso;

  {
  	struct smacq_optval optvals[] = {
		{"d", &idrsso},
		{"r", &rsso},
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

	idrss = idrsso.boolean_t;
	rss = rsso.boolean_t;
  }

  if (idrss) 
  	idrss_field = dts->requirefield("idrss");
  if (rss) 
  	rss_field = dts->requirefield("rss");

  longtype = dts->requiretype("uint32");
}
