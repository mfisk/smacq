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

/* Programming constants */

#define VECTORSIZE 21
#define ALARM_BITS 15
#define KEYBYTES 128

static struct smacq_options options[] = {
  {"pdf", {boolean_t:0}, "Report probabilities rather than absolute counts", SMACQ_OPT_TYPE_BOOLEAN},
  {"a", {boolean_t:0}, "Annotate and pass all objects instead of just the last", SMACQ_OPT_TYPE_BOOLEAN},
  {"f", {string_t:"count"}, "Name of field to store count in", SMACQ_OPT_TYPE_STRING},
  {NULL, {string_t:NULL}, NULL, 0}
};

SMACQ_MODULE(count, 
  PROTO_CTOR(count);
  PROTO_DTOR(count);
  PROTO_CONSUME();
  PROTO_PRODUCE();

  void annotate(DtsObject * datum, int c);

  struct fieldset fieldset;
  struct iovec_hash *counters;

  int counter;

  int prob; // Report probabilities

  int all;  // -a flag
  DtsObject * lastin;
  
  dts_field timefield; // Field number
  dts_field probfield; 
  dts_field countfield;
  int counttype;
  int probtype;
); 
  
void countModule::annotate(DtsObject * datum, int c) {
  if (prob) {
    double p = (double)c / counter;
    DtsObject * msgdata = dts->construct(probtype, &p);
    datum->attach_field(probfield, msgdata); 
  } else {
    DtsObject * msgdata = dts->construct(counttype, &c);
    datum->attach_field(countfield, msgdata); 
  }
}
 
smacq_result countModule::consume(DtsObject * datum, int * outchan) {
  int c = ++counter;

  if (fieldset.num) {
    struct iovec * domainv = datum->fields2vec(&fieldset);

    if (!domainv) {
      //fprintf(stderr, "Skipping datum\n");
      return SMACQ_FREE;
    }

    c = bytes_hash_table_incrementv(counters, domainv, fieldset.num);
    c++;
  }

  if (!all) {
    if (lastin) {
	    lastin->decref();
    }
    datum->incref();
    lastin = datum;

    return SMACQ_FREE;
  } 

  annotate(datum, c);

  return SMACQ_PASS;
}

countModule::countModule(struct smacq_init * context) : SmacqModule(context) {
  int argc = 0;
  char ** argv = NULL;
  smacq_opt probability, countopt, allflag;

  {
  	struct smacq_optval optvals[] = {
		{"pdf", &probability},
		{"f", &countopt},
		{"a", &allflag},
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

	prob = probability.boolean_t;
	all = allflag.boolean_t;
  }

  // Consume rest of arguments as fieldnames
  dts->fields_init(&fieldset, argc, argv);

  timefield = dts->requirefield("timeseries");
  if (prob) {
  	probfield = dts->requirefield("probability");
  	probtype = dts->requiretype("double");
  } else {
  	countfield = dts->requirefield(countopt.string_t);
  	counttype = dts->requiretype("int");
  }

  if (fieldset.num) 
  	counters = bytes_hash_table_new(KEYBYTES, CHAIN|NOFREE);
}

countModule::~countModule() {
  //fprintf(stderr, "count_shutdown!\n");
  //
  if (prob) {
  	dts_field_free(probfield);
  } else {
  	dts_field_free(countfield);
  }

  if (fieldset.num) 
  	bytes_hash_table_destroy(counters);

  fieldset_destroy(&fieldset);
}


smacq_result countModule::produce(DtsObject ** datump, int * outchan) {
  int c;
  struct iovec * domainv;

  if (!lastin) {
	return SMACQ_FREE;
  }

  domainv = lastin->fields2vec(&fieldset);
  *datump = lastin;
  lastin = NULL;

  if (fieldset.num) {
	assert("count: If field names are specified, you must use -a\n!");
  	c = (int)bytes_hash_table_lookupv(counters, domainv, fieldset.num);
  } else {
	c = counter;
  }

  assert(c!=0);
  annotate(*datump, c);

  //fprintf(stderr, "count_produce() %p\n", *datump);
  return (smacq_result)SMACQ_PASS|SMACQ_END;
}
