#include <assert.h>
#include <SmacqModule.h>
#include <sys/time.h>
#include <time.h>

static struct smacq_options options[] = {
  {"f", {string_t:"now"}, "Field name", SMACQ_OPT_TYPE_STRING},
  END_SMACQ_OPTIONS
};

SMACQ_MODULE(now, 
  PROTO_CTOR(now);
  PROTO_CONSUME();
 
  DtsField now_field;
  dts_typeid timeval_type;
); 
  
smacq_result nowModule::consume(DtsObject datum, int & outchan) {
	DtsObject msgdata = dts->newObject(timeval_type);
	gettimeofday((struct timeval*)msgdata->getdata(), NULL);
	datum->attach_field(now_field, msgdata); 

	return SMACQ_PASS;
}

nowModule::nowModule(struct SmacqModule::smacq_init * context) 
  : SmacqModule(context)
{
	int argc;
	char ** argv;

	smacq_opt nowfield_name;

 	struct smacq_optval optvals[] = {
		{"now", &nowfield_name},
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

	now_field = dts->requirefield(nowfield_name.string_t);
	timeval_type = dts->requiretype("timeval");
}

