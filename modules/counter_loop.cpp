#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
#include <fields.h>
#include <bytehash.h>
#include <ThreadedSmacqModule.h>

/* Programming constants */
#define KEYBYTES 128

static struct smacq_options options[] = {
  {"pdf", {boolean_t:0}, "Report probabilities rather than absolute counts", SMACQ_OPT_TYPE_BOOLEAN},
  {"f", {string_t:"counter"}, "Name of field to store count in", SMACQ_OPT_TYPE_STRING},
  {NULL, {string_t:NULL}, NULL, 0}
};

SMACQ_MODULE_THREAD(counter,); 

smacq_result counterModule::thread(struct smacq_init * context) {
  struct fieldset fieldset;
  struct iovec_hash *counters;

  int counter;

  dts_field timefield; // Field number
  dts_field probfield; 
  dts_field countfield;
  int counttype;
  int probtype;
  DtsObject * datum;
  int c;
  int argc = 0;
  char ** argv;
  smacq_opt probability, countfieldo;

  c = ++counter;

  struct smacq_optval optvals[] = {
	{"pdf", &probability},
	{"f", &countfieldo},
    	{NULL, NULL}
  };
  smacq_getoptsbyname(context->argc-1, context->argv+1,
		       &argc, &argv,
		       options, optvals);

  int doprob = probability.boolean_t;

  // Consume rest of arguments as fieldnames
  dts->fields_init(&fieldset, argc, argv);

  timefield = dts->requirefield("timeseries");
  if (doprob) {
  	probfield = dts->requirefield("probability");
  	probtype = dts->requiretype("double");
  } else {
  	countfield = dts->requirefield(countfieldo.string_t);
  	counttype = dts->requiretype("int");
  }

  counters = bytes_hash_table_new(KEYBYTES, CHAIN|NOFREE);

  //fprintf(stderr, "count thread is running\n");

  while( (datum = smacq_read()) ) {
    //fprintf(stderr, "count thread got datum\n");

    if (fieldset.num) {
      struct iovec * domainv = datum->fields2vec(&fieldset);

      if (!domainv) {
        //fprintf(stderr, "Skipping datum\n");
        return SMACQ_FREE;
      }

      c = bytes_hash_table_incrementv(counters, domainv, fieldset.num);
      c++;
    }

    if (doprob) {
      double p = (double)c / counter;
      DtsObject * msgdata = dts->construct(probtype, &p);
      datum->attach_field(probfield, msgdata); 
    } else {
      DtsObject * msgdata = dts->construct(counttype, &c);
      datum->attach_field(countfield, msgdata); 
    }
   
    smacq_decision(datum, SMACQ_PASS);
  }

  /* Shutdown */
  bytes_hash_table_destroy(counters);

  //fprintf(stderr, "Count thread exiting\n");

  return SMACQ_END;
}

