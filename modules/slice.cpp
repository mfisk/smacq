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

/* Programming constants */

#define VECTORSIZE 21
#define ALARM_BITS 15
#define KEYBYTES 128

static struct smacq_options options[] = {
  {"s", {string_t:NULL}, "Start point", SMACQ_OPT_TYPE_STRING},
  {"e", {string_t:NULL}, "End point", SMACQ_OPT_TYPE_STRING},
  END_SMACQ_OPTIONS
};

struct state {
  DTS* env
  
  void * startp, * endp;
  int startlen, endlen;
  int starttype, endtype;
}; 



static void printout(struct state * state) {
  if (!fieldvec.num) { 
    if (hasinterval)
      printf("%lu.%06lu\t%lu\n", nextinterval.tv_sec, nextinterval.tv_usec, counter);
    else 
      printf("%lu\n", counter);

  } else {
    if (hasinterval) 
      printf("\fCounters at %lu.%06lu\n", nextinterval.tv_sec, nextinterval.tv_usec);

    counters->foreach(print_counter, state);
  }
}
  
smacq_result sliceModule::consume(DtsObject datum, int & outchan) {
  int c;

  if (hasinterval) {
    DtsObject ts;
    int type;

    if (! (ts =datum->getfield(ts_field)) {
      fprintf(stderr, "error: timeseries not available\n");
    } else {
      if (!istarted) {
	istarted = 1;
	nextinterval = get_data_as(ts, struct timeval);
	timeval_inc(&nextinterval, interval);
      } else if (timeval_ge(dts_data_as(ts, struct timeval), nextinterval)) {
	// Print counters
	printout(state);

	timeval_inc(&nextinterval, interval);
	while (timeval_past(dts_data_as(ts, struct timeval) nextinterval)) { // gap in timeseries
	  timeval_inc(&nextinterval, interval);
	}
      }
    }
  }

  if (!fieldvec.num) {
    counter++;
  } else {
    fieldvec.getfields(datum);

    if (!domainv) {
      //fprintf(stderr, "Skipping datum\n");
      return SMACQ_FREE;
    }

    c = counters->increment(fieldvec);
  }

  return SMACQ_PASS;
}

sliceModule::sliceModule(struct SmacqModule::smacq_init * context) {
  int argc = 0;
  char ** argv;

  {
	smacq_opt interval;

  	struct smacq_optval optvals[] = {
    		{ "t", &interval}, 
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

	interval = interval.timeval_t;
	if ((interval.timeval_t.tv_sec != 0) || (interval.timeval_t.tv_usec != 0)) {
	  hasinterval = 1;
	} else {
	  hasinterval = 0;
	}
  }

  // Consume rest of arguments as fieldnames
  dts->fields_init(argc, argv);

  counters = new BytesHashTable(KEYBYTES, CHAIN|NOFREE);

  return 0;
}

sliceModule::~sliceModule(struct state * state) {
  // Print counters
  printout(state);
  return 0;
}


smacq_result sliceModule::produce(DtsObject & datum, int & outchan) {
  return SMACQ_ERROR;
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_slice_table = {
  &slice_produce, 
  &slice_consume,
  &slice_init,
  &slice_shutdown
};
