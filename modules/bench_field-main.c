#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
#include <fields.h>
#include "bytehash.h"

#define LOOPSIZE 1e7

extern void virtual_bench(int);
extern int function_bench(int);

static struct smacq_options options[] = {
  {"f", {string_t:"ts"}, "Field to get", SMACQ_OPT_TYPE_STRING},
  {NULL, {string_t:NULL}, NULL, 0}
};

struct state {
  smacq_environment * env;
  dts_field field;
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

static smacq_result bench_field_consume(struct state * state, const dts_object * datum, int * outchan) {
  const dts_object * x;
  double val;
  double len_cache, len_nocache, len_fn;
  struct timeval tvstart, tvstop;
  int i;

  if (! (x = smacq_getfield(state->env, datum, state->field, NULL))) {
      fprintf(stderr, "bench_field: no field\n");
      return SMACQ_PASS;
  }

  gettimeofday(&tvstart, NULL);
 
  for (i=0; i<LOOPSIZE; i++) { 
    x = smacq_getfield(state->env, datum, state->field, NULL);
    val = dts_data_as(x, double);
  }

  gettimeofday(&tvstop, NULL);
  len_cache = tv_diff(tvstart, tvstop);

  /* Uncached */
  gettimeofday(&tvstart, NULL);
 
  for (i=0; i<LOOPSIZE; i++) { 
    dts_fieldcache_flush(datum, dts_field_first(state->field));
    x = smacq_getfield(state->env, datum, state->field, NULL);
    val = dts_data_as(x, double);
    dts_decref(x);
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

static smacq_result bench_field_init(struct smacq_init * context) {
  int argc = 0;
  char ** argv;
  struct state * state = context->state = g_new0(struct state, 1);
  state->env = context->env;


  {
    smacq_opt field;

    struct smacq_optval optvals[] = {
      {"f", &field},
      {NULL, NULL}
    };

    smacq_getoptsbyname(context->argc-1, context->argv+1,
		       &argc, &argv,
		       options, optvals);

    state->field = smacq_requirefield(state->env, field.string_t);
  }

  return 0;
}

static smacq_result bench_field_shutdown(struct state * state) {
  free(state);
  return 0;
}


static smacq_result bench_field_produce(struct state * state, const dts_object ** datum, int * outchan) {
  return SMACQ_END;
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_bench_field_table = {
  &bench_field_produce, 
  &bench_field_consume,
  &bench_field_init,
  &bench_field_shutdown
};
