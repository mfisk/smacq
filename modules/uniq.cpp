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
#include <bloom.h>
#include <bytehash.h>

/* Programming constants */

#define VECTORSIZE 21
#define ALARM_BITS 15
#define KEYBYTES 128

SMACQ_MODULE(uniq,
	PROTO_CTOR(uniq);
	PROTO_DTOR(uniq);
	PROTO_CONSUME();

	     struct fieldset fieldset;
	     struct iovec_hash *drset;
	     struct bloom_summary *summary;
	     double prob; // Use probabilistic algorithms?
	     int use_obj_id;
);

static struct smacq_options options[] = {
  {"m", {double_t:0}, "Max amount of memory (MB) (forces probabilistic mode)", SMACQ_OPT_TYPE_DOUBLE},
  {"o", {boolean_t:0}, "Use object ID instead of fields", SMACQ_OPT_TYPE_BOOLEAN},
  {NULL, {string_t:NULL}, NULL, 0}
};

/*
 * Check presense in set.
 */
smacq_result uniqModule::consume(DtsObject * datum, int * outchan) {
  struct iovec obj_domainv[2];
  struct iovec * domainv;
  int numfields = fieldset.num;
  
  if (use_obj_id) {
	  domainv = obj_domainv;
	  unsigned long long base = datum->getid();
	  obj_domainv[0].iov_base = (char *)&base;
	  obj_domainv[0].iov_len = sizeof(unsigned long long);
	  numfields = 1;
  } else {
	  domainv = datum->fields2vec(&fieldset);
  }

  if (!domainv) {
    //fprintf(stderr, "Skipping datum\n");
    return SMACQ_FREE;
  }

  if (!prob) {
    if (!bytes_hash_table_setv(drset, domainv, numfields, (void*)1)) 
      // New entry in set
      return SMACQ_PASS;
  } else {
    if (bloom_insertv(summary, domainv, numfields)) 
      return SMACQ_PASS;
  }
    
  return SMACQ_FREE;
}

uniqModule::uniqModule(struct smacq_init * context) : SmacqModule(context) {
  int argc;
  char ** argv = NULL;

  {
	smacq_opt prob_opt, obj_opt;

  	struct smacq_optval optvals[] = {
    		{ "m", &prob_opt}, 
    		{ "o", &obj_opt}, 
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

	prob = prob_opt.double_t;
	use_obj_id = obj_opt.boolean_t;
  }

  // Consume rest of arguments as fieldnames
  dts->fields_init(&fieldset, argc, argv);

  if (!prob) {
    drset = bytes_hash_table_new(KEYBYTES, CHAIN|NOFREE);
  } else {
		// summary argument is number of bits, module argument is MB
    summary = bloom_summary_init(KEYBYTES, (long long unsigned)(prob * 1024 * 1024 * 8));
  }
}

uniqModule::~uniqModule() {
  if (!prob) {
    bytes_hash_table_destroy(drset);
  } else {
    //XXX: bloom_summary_destroy(summary);
  }

  fieldset_destroy(&fieldset);
}

