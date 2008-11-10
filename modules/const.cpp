#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define SMACQ_MODULE_IS_STATELESS 1

#include <SmacqModule.h>

static struct smacq_options options[] = {
	{"t", {string_t:"string"}, "Type", SMACQ_OPT_TYPE_STRING},
	{"f", {string_t:"const"}, "Name of annotation field", SMACQ_OPT_TYPE_STRING},
	END_SMACQ_OPTIONS
};

SMACQ_MODULE(const,
  PROTO_CTOR(const);
  PROTO_CONSUME();

  DtsField field;
  DtsObject data;
);

smacq_result constModule::consume(DtsObject datum, int & outchan) {
  datum->attach_field(field, data);
  return SMACQ_PASS;
}

constModule::constModule(struct SmacqModule::smacq_init * context) : SmacqModule(context) {
  int argc;
  const char ** argv;
  smacq_opt type_opt, field_opt;

  struct smacq_optval optvals[] = {
	  {"t", &type_opt},
	  {"f", &field_opt},
	  {NULL, NULL}
  };
  smacq_getoptsbyname(context->argc-1, context->argv+1,
                      &argc, &argv,
                      options, optvals);

  assert(argc==1);

  data = dts->construct_fromstring(dts->requiretype(type_opt.string_t), argv[0]);
  assert(data);
  field = dts->requirefield(field_opt.string_t);
}

