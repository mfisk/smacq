#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <SmacqModule.h>
#include <smacq-parser.h>

SMACQ_MODULE(msgtest, 
  PROTO_CTOR(msgtest);
  PROTO_CONSUME();
  int argc;
  char ** argv;
);

static struct smacq_options options[] = {
  END_SMACQ_OPTIONS
};

/* 
 * This is abstraction violation.  comp_new() is currently in a different .h file.
 * We don't provide a string constant and instead prime the comparison object cache.
 */
smacq_result msgtestModule::consume(DtsObject datum, int & outchan) {
  dts_comparison * comp = comp_new(EQ, comp_operand(FIELD, "srcip"), comp_operand(CONST, ""));
  DtsObject msgdata, srcip;

  // datum = dts_writable(env, datum);
  
  /* Get current time as message data */
  msgdata = datum->getfield("timeseries");

  /* Get current address as matching criteria (msg destination) */
  srcip = datum->getfield("srcip");
  comp->op2->valueo = srcip->dup();
  
  /* Send it to everybody else */
  DtsField f = dts->requirefield("prior");
  msgdata->dup()->send(f, comp);

  return SMACQ_PASS;
}

msgtestModule::msgtestModule(struct SmacqModule::smacq_init * context) : SmacqModule(context) {
  {
    struct smacq_optval optvals[] = {
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &argc, &argv,
				 options, optvals);
    assert(argv);
  }
}


