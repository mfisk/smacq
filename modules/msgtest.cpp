#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <smacq.h>
#include <smacq-parser.h>

SMACQ_MODULE(msgtest, 
  PROTO_CTOR(msgtest);
  PROTO_CONSUME();
  int argc;
  char ** argv;
);

static struct smacq_options options[] = {
  {NULL, {string_t:NULL}, NULL, 0}
};

/* 
 * This is abstraction violation.  comp_new() is currently in a different .h file.
 * We don't provide a string constant and instead prime the comparison object cache.
 */
smacq_result msgtestModule::consume(DtsObject * datum, int * outchan) {
  dts_comparison * comp = comp_new(EQ, comp_operand(FIELD, "srcip"), comp_operand(CONST, ""));
  DtsObject * msgdata, * srcip;

  // datum = dts_writable(env, datum);
  
  /* Get current time as message data */
  msgdata = datum->getfield(dts->requirefield("timeseries"));

  /* Get current address as matching criteria (msg destination) */
  srcip = datum->getfield(dts->requirefield("srcip"));
  comp->op2->valueo = srcip->dup();
  srcip->decref();

  /* Send it to everybody else */
  msgdata->dup()->send(dts->requirefield("prior"), comp);

  msgdata->decref();

  return SMACQ_PASS;
}

msgtestModule::msgtestModule(struct smacq_init * context) : SmacqModule(context) {
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



