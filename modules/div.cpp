#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define SMACQ_MODULE_IS_ANNOTATION 1
#define SMACQ_MODULE_IS_STATELESS 1
#include <SmacqModule.h>
#include <FieldVec.h>

static struct smacq_options options[] = {
  {"d", {double_t:1}, "Divisor", SMACQ_OPT_TYPE_DOUBLE},
  END_SMACQ_OPTIONS
};

SMACQ_MODULE(div,
  PROTO_CTOR(div);
  PROTO_CONSUME();

  DtsField timefield;
  DtsField divfield;
  int ticktype;
  double divisor;

  int current_tick;
  DtsObject ticko;
);

smacq_result divModule::consume(DtsObject datum, int & outchan) {
  double ts;
  int tick;

  DtsObject time = datum->getfield(timefield);
  if (!time) return SMACQ_PASS;

  ts = dts_data_as(time, double);
  tick = (int)(ts / divisor);

  if ( tick != current_tick) {
  	ticko = dts->construct(ticktype, &tick); 
  	assert(ticko);
	current_tick = tick;
  }
  datum->attach_field(divfield, ticko);

  return SMACQ_PASS;
}

divModule::divModule(smacq_init * context) : SmacqModule(context) {
  int argc = 0;
  char ** argv;

  {
    smacq_opt div_opt;

    struct smacq_optval optvals[] = {
      {"d", &div_opt},
      {NULL, NULL}
    };

    smacq_getoptsbyname(context->argc-1, context->argv+1,
		       &argc, &argv,
		       options, optvals);

    divisor = div_opt.double_t;
  }

  assert(argc);
  timefield = dts->requirefield(dts_fieldname_append(argv[0],"double"));
  divfield = dts->requirefield("div");
  ticktype = dts->requiretype("int");
}

