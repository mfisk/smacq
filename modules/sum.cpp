#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define SMACQ_MODULE_IS_ANNOTATION 1
#include <SmacqModule.h>
#include <FieldVec.h>

static struct smacq_options options[] = {
  {"a", {boolean_t:0}, "Output running total with each record", SMACQ_OPT_TYPE_BOOLEAN},
  {"f", {string_t:"sum"}, "Name of field to store result in", SMACQ_OPT_TYPE_STRING},
  END_SMACQ_OPTIONS
};

SMACQ_MODULE(sum, 
  PROTO_CTOR(sum);
  PROTO_DTOR(sum);
  PROTO_CONSUME();

  FieldVec fieldvec;

  double total;
  DtsField xfield;
  char * xfieldname;

  int sumtype;
  DtsField sumfield;
  int refreshtype;
  
  int outputall;
  DtsObject lastin;
); 
 
smacq_result sumModule::consume(DtsObject datum, int & outchan) {
  DtsObject newx;
  DtsObject msgdata;
  
  if (datum->gettype() != refreshtype) {
    if (! (newx = datum->getfield(xfield))) {
      fprintf(stderr, "sum: no %s field\n", xfieldname);
      return SMACQ_PASS;
    }
    
    // assert(newx.type == doubletype);

    total += dts_data_as(newx, double);
    
  }

  if (outputall || (datum->gettype() == refreshtype)) {
    msgdata = dts->construct(sumtype, &total);
    datum->attach_field(sumfield, msgdata); 
	
    return SMACQ_PASS;
  } else {
    lastin = datum;
    

    return SMACQ_FREE;
  }
    
}

sumModule::sumModule(struct SmacqModule::smacq_init * context) : SmacqModule(context) {
  int argc = 0;
  char ** argv;

  smacq_opt outputallo, fieldo;

  struct smacq_optval optvals[] = {
      {"a", &outputallo},
      {"f", &fieldo},
      {NULL, NULL}
  };

  smacq_getoptsbyname(context->argc-1, context->argv+1,
	       &argc, &argv,
	       options, optvals);

  outputall = outputallo.boolean_t;

  assert(argc==1);

  refreshtype = dts->requiretype("refresh");
  sumtype = dts->requiretype("double");
  sumfield = dts->requirefield(fieldo.string_t);
  
  xfieldname = dts_fieldname_append(argv[0], "double");
  xfield = dts->requirefield(xfieldname);
}

sumModule::~sumModule() {
  if (lastin) {
        DtsObject msgdata = dts->construct(sumtype, &total);
        lastin->attach_field(sumfield, msgdata); 

		enqueue(lastin);
  }
}

