#include <SmacqModule.h>
#include <smacq-parser.h>

static struct smacq_options options[] = {
  {"f", {string_t:"expr"}, "Name of field to store result in", SMACQ_OPT_TYPE_STRING},
  END_SMACQ_OPTIONS
};

SMACQ_MODULE(expr,
  PROTO_CTOR(expr);
  PROTO_CONSUME();

  int argc;
  char ** argv;

  DtsField expr_field;
  DtsField as_field;
  bool use_as_field;
  struct dts_operand * expr;

  int double_type;
);

exprModule::exprModule(struct SmacqModule::smacq_init * context)
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

  if ((argc > 2) && (!strcmp(argv[argc-2], "as"))) {
    fprintf(stderr, "expr got as %s\n", argv[argc-1]);
    as_field = dts->requirefield(argv[argc-1]);
    use_as_field = true;
    argc -= 2;
  } else {
    use_as_field = false;
  }

  expr = dts->parse_expr(argc, argv);
  if (!expr)
	assert(0);

  expr_field = dts->requirefield(fieldname.string_t);
}

smacq_result exprModule::consume(DtsObject datum, int & outchan) {
  DtsObject msgdata;
  double val;

  assert(datum);

  val = datum->eval_arith_operand(expr);

  msgdata = dts->construct(double_type, &val);

  //dts_data_as(msgdata, double) = eval_arith_operand(datum, op);

  datum->attach_field(expr_field, msgdata);

  if (use_as_field) {
    datum->attach_field(as_field, msgdata); 
  }

  return SMACQ_PASS;
}
