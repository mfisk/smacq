/*
 * This module filters data to find the most frequent values of a field or fields
 */

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
#include <bloom.h>
#include <FieldVec.h>

/* Programming constants */
#define KEYBYTES 128

SMACQ_MODULE(top, 
  PROTO_CTOR(top);
  PROTO_CONSUME();

  FieldVec fieldvec;
  FieldVecHash<int> counters;
  FieldVecBloomCounters * pcounters;
  double prob; // Use probabilistic algebraorithms?
  int threshold;

  // Non-probabilisitic only:
  int numentries;
  int totalcount;

  int do_count;
  int do_filter;

  dts_field count_field;
  int int_type;
);

static struct smacq_options options[] = {
  {"m", {double_t:0}, "Max amount of memory (MB) (forces probabilistic mode)", SMACQ_OPT_TYPE_DOUBLE},
  {"r", {double_t:1}, "Minimum ratio to average", SMACQ_OPT_TYPE_DOUBLE},
  {"f", {string_t:"pcount"}, "Count field", SMACQ_OPT_TYPE_STRING},
  END_SMACQ_OPTIONS
};

/*
 * Check presense in set.
 */
smacq_result topModule::consume(DtsObject datum, int & outchan) {
  double deviation;
  int val;

  fieldvec.getfields(datum);

  if (!prob) {
    val = ++counters[fieldvec];

    if (val == 1) numentries++;   // New entry
    
    totalcount++;
  } else {
    val = pcounters->increment(fieldvec);
  }

  if (do_count) {
    DtsObject msgdata = dts->construct(int_type, &val);
    datum->attach_field(count_field, msgdata);   
  }

  if (!do_filter) return SMACQ_PASS;

  /* Compute deviation */
  if (prob) {
        deviation = pcounters->deviation(val);
  } else {
        deviation = (double)(val) * (double)numentries / (double)totalcount;
  }

  if (deviation > threshold) {
    //printf("info: top: deviation is %g\n", deviation);
    return SMACQ_PASS;
  } else {
    return SMACQ_FREE;
  }
}

topModule::topModule(struct smacq_init * context) : SmacqModule(context) {
  int argc;
  char ** argv;
  smacq_opt pcount;

  {
	smacq_opt prob_opt, thresh;

  	struct smacq_optval optvals[] = {
    		{ "m", &prob_opt}, 
    		{ "r", &thresh}, 
		{ "f", &pcount},
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

	prob = (int) prob_opt.double_t;
	threshold = (int) thresh.double_t;
  }

  // Consume rest of arguments as fieldnames
  fieldvec.init(dts, argc, argv);

  if (prob) {
    pcounters = new FieldVecBloomCounters(prob/4 * 1024 * 1024);
  }

  do_filter = (threshold == 0);

  if (strcmp(pcount.string_t,  "")) {
    count_field = dts->requirefield(pcount.string_t);
    int_type = dts->requiretype("int");
    do_count = 1;
  } else {
    do_count = 0;
  }
}

