#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define SMACQ_MODULE_IS_ANNOTATION 1
#include <smacq.h>
#include <FieldVec.h>
#include <FieldVec.h>

static struct smacq_options options[] = {
  {"a", {boolean_t:0}, "Output sum only on refresh", SMACQ_OPT_TYPE_BOOLEAN},
  END_SMACQ_OPTIONS
};

SMACQ_MODULE(sum, 
  PROTO_CTOR(sum);
  PROTO_CONSUME();
  PROTO_PRODUCE();

  FieldVec fieldvec;

  double total;
  dts_field xfield;
  char * xfieldname;

  int sumtype;
  dts_field sumfield;
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

  {
    smacq_opt outputallo;

    struct smacq_optval optvals[] = {
      {"a", &outputallo},
      {NULL, NULL}
    };

    smacq_getoptsbyname(context->argc-1, context->argv+1,
		       &argc, &argv,
		       options, optvals);

    outputall = outputallo.boolean_t;
  }

  assert(argc==1);

  refreshtype = dts->requiretype("refresh");
  sumtype = dts->requiretype("double");
  sumfield = dts->requirefield("sum");
  
  xfieldname = dts_fieldname_append(argv[0], "double");
  xfield = dts->requirefield(xfieldname);
}

smacq_result sumModule::produce(DtsObject & datum, int & outchan) {
  if (lastin) {
        DtsObject msgdata = dts->construct(sumtype, &total);
        lastin->attach_field(sumfield, msgdata); 
	
	datum = lastin;
	lastin = NULL;
  	//fprintf(stderr, "sum last call for %p\n", datum);
	return SMACQ_PASS|SMACQ_END;
  } else {
  	return SMACQ_FREE|SMACQ_END;
  }
}

