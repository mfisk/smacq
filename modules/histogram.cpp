#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <SmacqModule.h>
#include <SmacqGraph.h>

static struct smacq_options options[] = {
  {"u", {boolean_t:0}, "Count unique members only", SMACQ_OPT_TYPE_BOOLEAN},
  {"t", {double_t:0}, "Compute histograms for time bins of the given duration", SMACQ_OPT_TYPE_DOUBLE},
  {"f", {string_t:""}, "Time field to use", SMACQ_OPT_TYPE_STRING},
  END_SMACQ_OPTIONS
};

SMACQ_MODULE(histogram,
  PROTO_CTOR(histogram);
);

histogramModule::histogramModule(struct SmacqModule::smacq_init * context)
  : SmacqModule(context)
{
  int argc;
  char ** argv;

  smacq_opt uniq, timebin, timefield;

  struct smacq_optval optvals[] = {
               {"f", &timefield},
               {"t", &timebin},
               {"u", &uniq},
               {NULL, NULL}
  };
  smacq_getoptsbyname(context->argc-1, context->argv+1,
                               &argc, &argv,
                               options, optvals);

  assert(argc);
  std::string args(argv2string(argc, argv));
  std::string query;

  if (uniq.boolean_t) {
  	query = "(count() from uniq(";
	query += args + ")) group by " + args;
  } else {
  	query = "count() group by -g " + args;
  }

  if (timebin.double_t) {
	query += ", clock(";
	query += timefield.string_t;
	query += ")";
  }

  fprintf(stderr, "Histogram translated to: %s\n", query.c_str());
  SmacqGraphContainer g;
  g.addQuery(dts, context->scheduler, query);
  assert(!g.empty());
  g.init(dts, context->scheduler);
  context->self->replace(&g);
}
