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
  char *args = argv2str(argc, argv);
  char *query = (char*)malloc(strlen(args)*2 + strlen(timefield.string_t) + 256);

  if (uniq.boolean_t) {
  	sprintf(query, "(count() from uniq(%s)) group by %s", args, args);
  } else {
  	sprintf(query, "count() group by -g %s", args);
  }

  if (timebin.double_t) {
	strcat(query, ", clock(");
	strcat(query, timefield.string_t);
	strcat(query, ")");
  }

  fprintf(stderr, "Histogram translated to: %s\n", query);
  SmacqGraph * g = SmacqGraph::newQuery(dts, context->scheduler, 1, &query);
  assert(g);
  g->init_all(dts, context->scheduler);
  context->self->replace(g);
}
