//#include <stdio.h>
//#include <stdlib.h>
#include <string>
#include <SmacqModule.h>
#include <FastBit.h>
//#include <SmacqGraph.h>

SMACQ_MODULE(fastbit,
  PROTO_CTOR(fastbit);
  PROTO_PRODUCE();

  std::vector<float> hits;
  dts_typeid double_type;
  DtsField attribute_field;
  unsigned long numRows;
  processInvariants(SmacqGraph_ptr g);
  std::string where;
);

static struct smacq_options options[] = {
  {"f", {string_t:NULL}, "Input index file", SMACQ_OPT_TYPE_STRING},
  {"a", {string_t:NULL}, "Index attribute name", SMACQ_OPT_TYPE_STRING},
  END_SMACQ_OPTIONS
};

smacq_result disarmModule::produce(DtsObject & datump, int & outchan) {
  double value = hits[iterator++];
  datum = dts->construct(empty_type, NULL);
  datum->attach_field(attribute_field, dts->construct(double_type, &value));

  if (hits >= numRows) {
	return SMACQ_PASS|SMACQ_END;
  } else {
	return SMACQ_PASS|SMACQ_PRODUCE;
  }
}

fastbitModule::processInvariants(SmacqGraph_ptr g) {
  if (!g) return;

  int const argc = g->getArgc();
  char ** argv = g->getArgv();
 
  if (!strcmp(argv[0], "equals") && argc == 3) {
    if (where.length()) where += " && ";
    where += argv[1];
    where += "=";
    where += argv[2];
  } else if (!strcmp(argv[0], "filter") && argc == 2) {
    if (where.length()) where += " && ";
    where += argv[1];
  } else {
    fprintf(stderr, "Cannot eagerly perform %s (%d args)\n", argv[0]
, argc);
  }

  if (g->getChildren()[0].size()) 
    processInvariants(column, g->getChildren()[0][0].get());
}

fastbitModule::fastbitModule(struct SmacqModule::smacq_init * context) : SmacqModule(context) {
  smacq_opt infile, attribute;

  struct smacq_optval optvals[] = {
      { "f", &infile},
      { "a", &attribute},
      {NULL, NULL}
  };

  smacq_getoptsbyname(context->argc-1, context->argv+1,
				 NULL, NULL,
				 options, optvals);

  double_type = dts->requiretype("double");
  assert(infile.string_t);
  assert(attribute.string_t);
  attribute_field = dts->requirefield(attribute.string_t);
  processInvariants(context->self->getChildInvariants(dts, context->scheduler, attribute_field));
 
  FastBit::init();
  FastBit::evaluateQuery(infile.string_t, where.c_str(), attribute.string_t, hits);
  numRows = hits.size();
}
