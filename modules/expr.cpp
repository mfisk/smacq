#include <SmacqModule.h>
#include <smacq-parser.h>

static struct smacq_options options[] = {
  {"f", {string_t:"expr"}, "Name of field to store result in", SMACQ_OPT_TYPE_STRING},
  END_SMACQ_OPTIONS
};

SMACQ_MODULE(expr,
  PROTO_CTOR(expr);
  PROTO_CONSUME();

  DtsField expr_field;
  DtsField as_field;
  bool use_as_field;
  struct dts_operand * expr;

  int double_type;
);

exprModule::exprModule(smacq_init * context)
 : SmacqModule(context)
{
  int argc = 0;
  char ** argv;

  smacq_opt fieldname;

  struct smacq_optval optvals[] = {
      {"f", &fieldname},
      {NULL, NULL}
  };
  smacq_getoptsbyname(context->argc-1, context->argv+1,
               &argc, &argv,
               options, optvals);
  
  double_type = dts->requiretype("double");
  expr_field = dts->requirefield(fieldname.string_t);

  assert(expr = dts->parse_expr(argv2string(argc, argv)));
}

smacq_result exprModule::consume(DtsObject datum, int & outchan) {
  double val = datum->eval_arith_operand(expr);
  DtsObject msgdata = dts->construct(double_type, &val);
  datum->attach_field(expr_field, msgdata);

  return SMACQ_PASS;
}
