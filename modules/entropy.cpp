#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <SmacqModule.h>

SMACQ_MODULE(entropy,
  PROTO_CTOR(entropy);
  PROTO_DTOR(entropy);
  PROTO_CONSUME();

  double total;
  double prev_total;

  DtsField probfield; 
  DtsField entropyfield;
  int refreshtype;
  int probtype;

  DtsObject lasto;
); 
 
static struct smacq_options options[] = {
  END_SMACQ_OPTIONS
};

smacq_result entropyModule::consume(DtsObject datum, int & outchan) {
	if (datum->gettype() == refreshtype) {
		double tot = total / log(2);
    		DtsObject msgdata = dts->construct(probtype, &tot);
		// fprintf(stderr, "Got refresh\n");
    		datum->attach_field(entropyfield, msgdata); 
		
		prev_total = total;
		total = 0;
	
		return SMACQ_PASS;
	} else {
		DtsObject probo;
		double prob;

		// Keep last datum around so we can use it to spit out data when we're ready
		// fprintf(stderr, "replacing cache of %p(%d) with %p(%d)\n", lasto, lasto ? lasto->gettype() : -1, datum, datum->gettype());
		assert(lasto != datum);
		if (lasto) 
		lasto = datum;

		if (!(probo = datum->getfield(probfield))) {
			fprintf(stderr, "No probability field\n");
			return SMACQ_PASS;
		}
		prob = dts_data_as(probo, double);
		
		total -= prob * log(prob);
  		
		return SMACQ_FREE;
	}
}

entropyModule::entropyModule(struct SmacqModule::smacq_init * context) 
  : SmacqModule(context) 
{
  {
  	int argc = 0;
  	const char ** argv;

  	struct smacq_optval optvals[] = {
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);
  }

  refreshtype = dts->requiretype("refresh");
  probfield = dts->requirefield(dts_fieldname_append("probability", "double"));
  probtype = dts->requiretype("double");
  entropyfield = dts->requirefield("entropy");
}

entropyModule::~entropyModule() {
  // fprintf(stderr, "TOTAL is %g\n", total);

  if (total && lasto) {
		double tot = total / log(2);
    		DtsObject msgdata = dts->construct(probtype, &tot);
		// fprintf(stderr, "Got refresh\n");
    		lasto->attach_field(entropyfield, msgdata); 
		
		prev_total = total;
		total = 0;
	
		enqueue(lasto);
  }
}

