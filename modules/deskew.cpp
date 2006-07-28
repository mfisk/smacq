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
  {"b", {double_t:0}, "Times must not be before this", SMACQ_OPT_TYPE_DOUBLE},
  {"e", {double_t:0}, "Times must not be after this", SMACQ_OPT_TYPE_DOUBLE},
  {"s", {string_t:NULL}, "Secondary time field to adjust by equal amount", SMACQ_OPT_TYPE_STRING},
  END_SMACQ_OPTIONS
};

SMACQ_MODULE(deskew,
  PROTO_CTOR(deskew);
  PROTO_CONSUME();

  DtsField timeField;
  DtsField doubleField;
  DtsField secondaryField;
  double previousTs, begin, end;

  // Temp vars
  dts_typeid fieldType;
  double ts;

  protected:
    void correct(DtsObject datum, DtsField fieldname, double val);
    bool getField(DtsObject datum, DtsField fieldname);
);

void deskewModule::correct(DtsObject datum, DtsField fieldName, double val) {
  //Attach new field, same type as orig
  char tsString[256];
  snprintf(tsString, 255, "%f", val);
  DtsObject obj = dts->construct_fromstring(this->fieldType, tsString); 
  assert(obj);
  datum->attach_field(fieldName, obj);
}

bool deskewModule::getField(DtsObject datum, DtsField fieldname) {
  DtsObject obj = datum->getfield(fieldname);
  if (!obj) return false;

  this->fieldType = obj->gettype();

  obj = obj->getfield(doubleField);
  assert(obj);
  this->ts = dts_data_as(obj, double);

  return true;
}

smacq_result deskewModule::consume(DtsObject datum, int & outchan) {
  if (!this->getField(datum, timeField)) {
	return SMACQ_PASS;
  }

  //fprintf(stderr, "got something of type %s\n", dts->typename_bynum(this->fieldType));
  if ((begin && ts < begin) || (end && ts > end)) {
	if (!previousTs) previousTs = begin;
	double diff = previousTs - ts;
  	//fprintf(stderr, "got skew of %g seconds \n", diff);

	this->correct(datum, timeField, previousTs);

	if (secondaryField && this->getField(datum, secondaryField)) {
		this->correct(datum, secondaryField, this->ts + diff);
	}
  } else {
  	//fprintf(stderr, "no skew\n");
	previousTs = ts;	
  }

  return SMACQ_PASS;
}

deskewModule::deskewModule(struct SmacqModule::smacq_init * context) : SmacqModule(context) {
  int argc = 0;
  char ** argv;

  {
    smacq_opt beginOpt, endOpt, secondaryOpt;

    struct smacq_optval optvals[] = {
      {"b", &beginOpt},
      {"e", &endOpt},
      {"s", &secondaryOpt},
      {NULL, NULL}
    };

    smacq_getoptsbyname(context->argc-1, context->argv+1,
		       &argc, &argv,
		       options, optvals);

    if (secondaryOpt.string_t) 
  	secondaryField = dts->requirefield(secondaryOpt.string_t);

    begin = beginOpt.double_t;
    end = endOpt.double_t;
  }

  assert(argc == 1);
  timeField = dts->requirefield(argv[0]);
  doubleField = dts->requirefield("double");

}

