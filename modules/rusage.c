#include <stdlib.h>
#include <assert.h>
#include <smacq.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

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
  {NULL, {string_t:NULL}, NULL, 0}
};

struct state {
  smacq_environment * env;
  int longtype;

  int rss;
  int rss_field;

  int idrss;
  int idrss_field;
}; 
  
static smacq_result rusage_consume(struct state * state, const dts_object * datum, int * outchan) {
  struct rusage rusage;
  getrusage(RUSAGE_SELF, &rusage);

  if (state->idrss) {
    dts_object * msgdata = flow_dts_construct(state->env, state->longtype, &rusage.ru_idrss);
    dts_attach_field(datum, state->idrss_field, msgdata); 
  }
  if (state->rss) {
    dts_object * msgdata = flow_dts_construct(state->env, state->longtype, &rusage.ru_maxrss);
    dts_attach_field(datum, state->rss_field, msgdata); 
  }
 
  return SMACQ_PASS;
}

static int rusage_init(struct flow_init * context) {
  int argc = 0;
  char ** argv;
  smacq_opt idrss, rss;
  struct state * state = context->state = g_new0(struct state, 1);
  state->env = context->env;

  {
  	struct smacq_optval optvals[] = {
		{"d", &idrss},
		{"r", &rss},
    		{NULL, NULL}
  	};
  	flow_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

	state->idrss = idrss.boolean_t;
	state->rss = rss.boolean_t;
  }

  if (state->idrss) 
  	state->idrss_field = flow_requirefield(state->env, "idrss");
  if (state->rss) 
  	state->rss_field = flow_requirefield(state->env, "rss");

  state->longtype = flow_requiretype(state->env, "uint32");

  return 0;
}

static int rusage_shutdown(struct state * state) {
  return 0;
}


static smacq_result rusage_produce(struct state * state, const dts_object ** datum, int * outchan) {
  return SMACQ_ERROR;
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_rusage_table = {
  &rusage_produce, 
  &rusage_consume,
  &rusage_init,
  &rusage_shutdown
};
