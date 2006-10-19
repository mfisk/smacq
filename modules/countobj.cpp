#include <string.h>
#include <SmacqModule.h>

// This module is used when evaluating modules in parallel
// and a barrier is needed to make sure that an object only
// passes once a sufficient number of filters have passed an object.
SMACQ_MODULE(countobj,
	     PROTO_CTOR(countobj);
	     PROTO_CONSUME();

	     DtsField seen_field;
	     std::vector<DtsObject> seen_obj;
	     dts_typeid seen_type;

	int threshold;
);

static struct smacq_options options[] = {
  {"t", {double_t:0}, "An object will be filtered until it is seen this many times", SMACQ_OPT_TYPE_DOUBLE},
  END_SMACQ_OPTIONS
};

smacq_result countobjModule::consume(DtsObject datum, int & outchan) {
  DtsObject foo = datum->getfield(seen_field, true);
  int value;

  if (foo) {
    value = dts_data_as(foo, int);
    value++;
  } else {
    value = 1;
  }

  if (value == threshold) {
    // Should we increment again?
    return SMACQ_PASS;
  }

  datum->attach_field(seen_field, seen_obj[value]);
  foo = datum->getfield(seen_field);
  return SMACQ_FREE;
}

countobjModule::countobjModule(smacq_init * context) 
  : SmacqModule(context)
{
  smacq_opt threshold_opt;
  int argc; 
  char ** argv;

  struct smacq_optval optvals[] = {
      {"t", &threshold_opt},
      {NULL, NULL}
  };
  smacq_getoptsbyname(context->argc-1, context->argv+1,
                        &argc, &argv,
                        options, optvals);

  char seen_fieldstr[64];
  sprintf(seen_fieldstr, "count%p", this);
  seen_field = dts->requirefield(seen_fieldstr);
  seen_type = dts->requiretype("int");

  threshold = (int)threshold_opt.double_t;
  assert(threshold > 0);

  // Create counters statically
  seen_obj.resize(threshold);
  for (int i = 1; i < threshold; i++) {
	 seen_obj[i] = dts->construct(seen_type, &i); 
  }
}

