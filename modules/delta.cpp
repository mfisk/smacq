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

#define SMACQ_MODULE_IS_ANNOTATION 1

static struct smacq_options options[] = {
  {NULL, {string_t:NULL}, NULL, 0}
};

SMACQ_MODULE(delta,
  PROTO_CTOR(delta);
  PROTO_CONSUME();

  struct fieldset fieldset;
  struct iovec_hash *counters;
  int started;

  double lastx;
  dts_field xfield;
  char * xfieldname;

  int deltatype;
  dts_field deltafield;
); 
 
smacq_result deltaModule::consume(DtsObject * datum, int * outchan) {
  DtsObject * newx;

  if (! (newx = datum->getfield(xfield))) {
	fprintf(stderr, "delta: no %s field\n", xfieldname);
	return SMACQ_PASS;
  }

  // assert(newx->type == doubletype);

  if (started) {
	double delta = dts_data_as(newx, double) - lastx;
    	DtsObject * msgdata = dts->construct(deltatype, &delta);
    	datum->attach_field(deltafield, msgdata); 
  } else {
	started = 1;
  }

  lastx = dts_data_as(newx, double);
  newx->decref();
	
  return SMACQ_PASS;
}

deltaModule::deltaModule(struct smacq_init * context) : SmacqModule(context) {
  int argc = context->argc-1;
  char ** argv = context->argv+1;;

  assert(argc==1);

  deltatype = dts->requiretype("double");
  deltafield = dts->requirefield("delta");
  
  xfieldname = dts_fieldname_append(argv[0], "double");
  xfield = dts->requirefield(xfieldname);
}

