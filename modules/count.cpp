#include <stdlib.h>
#include <assert.h>
#include <SmacqModule.h>
#include <FieldVec.h>

static struct smacq_options options[] = {
  {"pdf", {boolean_t:0}, "Report probabilities rather than absolute counts", SMACQ_OPT_TYPE_BOOLEAN},
  {"a", {boolean_t:0}, "Annotate and pass all objects instead of just the last", SMACQ_OPT_TYPE_BOOLEAN},
  {"f", {string_t:"count"}, "Name of field to store count in", SMACQ_OPT_TYPE_STRING},
  END_SMACQ_OPTIONS
};

SMACQ_MODULE(count, 
  PROTO_CTOR(count);
  PROTO_CONSUME();
  PROTO_PRODUCE();

  void annotate(DtsObject datum, int c);

  FieldVec fieldvec;
  FieldVecHash<int> counters;

  int counter;

  int prob; // Report probabilities

  int all;  // -a flag
  DtsObject lastin;
  
  DtsField timefield; // Field number
  DtsField probfield; 
  DtsField countfield;
  int counttype;
  int probtype;
); 
  
void countModule::annotate(DtsObject datum, int c) {
  if (prob) {
    double p = (double)c / counter;
    DtsObject msgdata = dts->construct(probtype, &p);
    datum->attach_field(probfield, msgdata); 
  } else {
    DtsObject msgdata = dts->construct(counttype, &c);
    datum->attach_field(countfield, msgdata); 
  }
}
 
smacq_result countModule::consume(DtsObject datum, int & outchan) {
  int c;

  if (! fieldvec.empty()) {
    fieldvec.getfields(datum);

    c = ++counters[fieldvec];
    c++;
  } else {
    c = ++counter;
  }

  if (!all) {
    if (lastin) {
	    
    }
    
    lastin = datum;

    return SMACQ_FREE;
  } 

  annotate(datum, c);

  return SMACQ_PASS;
}

countModule::countModule(struct SmacqModule::smacq_init * context) 
  : SmacqModule(context), counter(0)
{
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
  fieldvec.init(dts, argc, argv);

  timefield = dts->requirefield("timeseries");
  if (prob) {
  	probfield = dts->requirefield("probability");
  	probtype = dts->requiretype("double");
  } else {
  	countfield = dts->requirefield(countopt.string_t);
  	counttype = dts->requiretype("int");
  }
}

smacq_result countModule::produce(DtsObject & datump, int & outchan) {
  //fprintf(stderr, "count_produce()\n");
  
  if (!lastin) {
    return SMACQ_FREE;
  }

  if (! fieldvec.empty()) {
    assert("count: If field names are specified, you must use -a\n!"&&0);
  } else {
    annotate(lastin, counter);
    datump = lastin;
    lastin = NULL;
  }

  //fprintf(stderr, "count_produce() %p\n", datump);
  return (SMACQ_PASS|SMACQ_END);
}
