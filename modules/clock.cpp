#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
#include <FieldVec.h>
#include <FieldVec.h>

static struct smacq_options options[] = {
  {"t", {double_t:1}, "Clock tick length", SMACQ_OPT_TYPE_DOUBLE},
  END_SMACQ_OPTIONS
};

#define SMACQ_MODULE_IS_ANNOTATION 1

SMACQ_MODULE(clock,
  PROTO_CTOR(clock);
  PROTO_CONSUME();
  PROTO_PRODUCE();

  dts_field timefield;
  dts_field clockfield;
  int refreshtype;
  int ticktype;
  double period;

  long int current_tick;
  DtsObject current_ticko;
);

smacq_result clockModule::consume(DtsObject datum, int & outchan) {
  double ts;
  long int tick;

  {
      	DtsObject time;
	time = datum->getfield(timefield);
  	if (!time) return SMACQ_PASS;

  	ts = dts_data_as(time, double);
  	
  }
  tick = (int)(ts / period);
  /* fprintf(stderr, "got time tick %ld (period %f)\n", tick, period); */

  /* only change clock forward */
  if ( tick > current_tick) {
  	DtsObject ticko; 
  	/* fprintf(stderr, "clock advanced to %ld (period %f)\n", tick, period); */

	DtsObject refresh = dts->newObject(refreshtype);
	refresh->attach_field(clockfield, current_ticko);
  	//fprintf(stderr, "clock is queueing %p (a refresh of type %d)\n", refresh, refreshtype);
	enqueue(refresh, outchan);

  	ticko = dts->construct(ticktype, &tick); 
  	assert(ticko);

	current_tick = tick;
	current_ticko = ticko;
  }

  datum->attach_field(clockfield, current_ticko);
  

  return (SMACQ_PASS|canproduce());
}

clockModule::clockModule(struct smacq_init * context) : SmacqModule(context) {
  int argc = 0;
  char ** argv;

  {
    smacq_opt period_opt;

    struct smacq_optval optvals[] = {
      {"t", &period_opt},
      {NULL, NULL}
    };

    smacq_getoptsbyname(context->argc-1, context->argv+1,
		       &argc, &argv,
		       options, optvals);

    period = period_opt.double_t;
  }

  assert(argc);
  timefield = dts->requirefield(dts_fieldname_append(argv[0],"double"));
  clockfield = dts->requirefield("clock");
  refreshtype = dts->requiretype("refresh");
  ticktype = dts->requiretype("int");
}

smacq_result clockModule::produce(DtsObject & datum, int & outchan) {
  //fprintf(stderr, "clock is producing (a refresh)\n");
  if (canproduce()) {
  	return dequeue(datum, outchan);
  } else {
	/* Forced last call */
	datum = dts->construct(refreshtype, NULL);
	datum->attach_field(clockfield, current_ticko);
	return (SMACQ_PASS|SMACQ_END);
  }
}

