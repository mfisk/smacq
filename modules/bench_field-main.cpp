#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
#include <FieldVec.h>
#include <FieldVec.h>

#define LOOPSIZE 1e7

extern void virtual_bench(int);
extern int function_bench(int);

static struct smacq_options options[] = {
  {"f", {string_t:"ts"}, "Field to get", SMACQ_OPT_TYPE_STRING},
  END_SMACQ_OPTIONS
};

struct state {
  DTS * env;
  DtsField field;
}; 

int get42(int x) {
	return x;
}

static double tv_diff(struct timeval tvstart, struct timeval tvstop) {
  int sec = tvstop.tv_sec - tvstart.tv_sec;
  int usec = tvstop.tv_usec - tvstart.tv_usec;

  if (usec < 0) {
	  sec--;
	  usec += 1e6;  
  }
  return (sec + (usec * 1e-6));
}

static smacq_result bench_field_consume DtsObject datum, int & outchan) {
  DtsObject x;
  double val;
  double len_cache, len_nocache, len_fn;
  struct timeval tvstart, tvstop;
  int i;

  if (! (x = datum->getfield(field))) {
      fprintf(stderr, "bench_field: no field\n");
      return SMACQ_PASS;
  }

  gettimeofday(&tvstart, NULL);
 
  for (i=0; i<LOOPSIZE; i++) { 
    x = datum->getfield(field);
    val = dts_data_as(x, double);
  }

  gettimeofday(&tvstop, NULL);
  len_cache = tv_diff(tvstart, tvstop);

  /* Uncached */
  gettimeofday(&tvstart, NULL);
 
  for (i=0; i<LOOPSIZE; i++) { 
    datum->fieldcache_flush(field);
    x = datum->getfield(field);
    val = dts_data_as(x, double);
    
  }

  gettimeofday(&tvstop, NULL);
  len_nocache = tv_diff(tvstart, tvstop);

  // Again 

  gettimeofday(&tvstart, NULL);
 
  for (i=0; i<LOOPSIZE; i++) { 
    val = function_bench(42);
  }

  gettimeofday(&tvstop, NULL);
  len_fn = tv_diff(tvstart, tvstop);

  fprintf(stderr, "%g ns/get, %g ns/cachedget, %g ns/fn, ",
	  len_nocache*1e9/LOOPSIZE, 
	  len_cache*1e9/LOOPSIZE, 
	  len_fn*1e9/LOOPSIZE
	  );

  virtual_bench(0);

  return SMACQ_PASS;
}

static smacq_result bench_field_init(struct SmacqModule::smacq_init * context) {
  int argc = 0;
  char ** argv;

  {
    smacq_opt field;

    struct smacq_optval optvals[] = {
      {"f", &field},
      {NULL, NULL}
    };

    smacq_getoptsbyname(context->argc-1, context->argv+1,
		       &argc, &argv,
		       options, optvals);

    field = dts->requirefield(field.string_t);
  }

  return 0;
}

static smacq_result bench_field_shutdown(struct state * state) {
  return 0;
}


static smacq_result bench_field_produce DtsObject datum, int & outchan) {
  return SMACQ_END;
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_bench_field_table = {
  &bench_field_produce, 
  &bench_field_consume,
  &bench_field_init,
  &bench_field_shutdown
};
