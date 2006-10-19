#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <SmacqModule.h>
#include <FieldVec.h>
#include <FieldVec.h>

static struct smacq_options options[] = {
  END_SMACQ_OPTIONS
};

#define MODULE_IS_ANNOTATION 1

SMACQ_MODULE(derivative, 
  PROTO_CTOR(derivative);
  PROTO_CONSUME();

  FieldVec fieldvec;
  int started;

  double lastx;
  double lasty;
  DtsField xfield;
  DtsField yfield;
  char * xfieldname;
  char * yfieldname;

  int derivtype;
  DtsField derivfield;
); 
 
smacq_result derivativeModule::consume(DtsObject datum, int & outchan) {
  DtsObject newx, newy;

  if (! (newx = datum->getfield(xfield))) {
	fprintf(stderr, "derivative: no %s field\n", xfieldname);
	return SMACQ_PASS;
  }
  if (! (newy = datum->getfield(yfield))) {
	fprintf(stderr, "derivative: no %s field\n", yfieldname);
	
	return SMACQ_PASS;
  }

  if (started) {
	double dydx = (dts_data_as(newy, double) - lasty) / 
			(dts_data_as(newx, double) - lastx);
    	DtsObject msgdata = dts->construct(derivtype, &dydx);
    	datum->attach_field(derivfield, msgdata); 
  } else {
	started = 1;
  }

  lastx = dts_data_as(newx, double);
  lasty = dts_data_as(newy, double);
	
  
  

  return SMACQ_PASS; 
}

derivativeModule::derivativeModule(smacq_init * context) : SmacqModule(context) {
  int argc = 0;
  char ** argv;
  {
  	struct smacq_optval optvals[] = {
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);
  }

  assert(argc==2);

  derivtype = dts->requiretype("double");
  derivfield = dts->requirefield("derivative");

  yfieldname = dts_fieldname_append(argv[0], "double");
  xfieldname = dts_fieldname_append(argv[1], "double");

  xfield = dts->requirefield(xfieldname);
  yfield = dts->requirefield(yfieldname);
}

