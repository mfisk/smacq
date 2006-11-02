#include <math.h>
#include <SmacqModule.h>
#include <DynamicArray.h>
#include <FieldVecDB.h>
#include <stdexcept>
#include <ndbm.h>
#include <errno.h>

static struct smacq_options options[] = {
  {"histup", {double_t:0}, "History when moving average up", SMACQ_OPT_TYPE_DOUBLE},
  {"histdown", {double_t:0.9995}, "History when moving average down", SMACQ_OPT_TYPE_DOUBLE},
  {"histdev", {double_t:0.8}, "Standard Deviation history", SMACQ_OPT_TYPE_DOUBLE},
  {"f", {string_t:"severity"}, "Report severity of change as this field", SMACQ_OPT_TYPE_STRING},
  {"t", {double_t:-1}, "Threshold (in standard deviations)", SMACQ_OPT_TYPE_DOUBLE},
  {"i", {string_t:NULL}, "Identify field", SMACQ_OPT_TYPE_STRING},
  {"a", {boolean_t:0}, "Add mean and sigma fields to each feature", SMACQ_OPT_TYPE_BOOLEAN},
  {"base", {double_t:1}, "Feature value is log with this base", SMACQ_OPT_TYPE_DOUBLE},
  {"s", {string_t:NULL}, "File to save/restore state from", SMACQ_OPT_TYPE_STRING},
  END_SMACQ_OPTIONS
};

class per_id_dimension_state {
public:
  per_id_dimension_state() : avg(0), stddev(0), distance(0), oldavg(0), oldstddev(0) {}
  
  double avg;
  double stddev;
  double distance;

  double oldavg;
  double oldstddev;
};

class per_dimension_state {
public:
  per_dimension_state(DTS * dts, char * field_name, const char * file_name) : id((std::string(file_name) + "-") + field_name) {
        field = dts->requirefield(dts_fieldname_append(field_name, "double"));
        raw_field = dts->requirefield(field_name);
  }

  FieldVecDB<per_id_dimension_state> id;
  DtsField field, raw_field;
  DtsObject last_field;
};

SMACQ_MODULE(changes,
	     PROTO_CTOR(changes);
	     PROTO_CONSUME();

	     bool attach_all;

	     double threshold;
	     int	dimensions;

	     double base;

	     double alpha_avg_up;
	     double alpha_avg_down;
	     double alpha_stddev;

	     DtsField id_field, severity_field, avg_field, sigma_field;
	     dts_typeid double_type;

	     std::vector<per_dimension_state> Dim;
	     );

smacq_result changesModule::consume(DtsObject datum, int & outchan) {
  double total_distance = 0;

  DtsObject id = datum->getfield(id_field);
  if (!id) {
    //fprintf(stderr, "ID field not present\n");
    return SMACQ_FREE;
  }

  for (int i = 0; i < dimensions; i++) {
    per_id_dimension_state d = Dim[i].id.get(id);
    DtsObject o = datum->getfield(Dim[i].field);
		
    // make sure we got it
    if (o) {
      double val = dts_data_as(o, double);
      if (base != 1) {
	val = pow(base,val);
      }
      double dev = fabs(d.avg - val);

      // Use first value as seed and don't call it anomalous
      if (!d.stddev && !d.avg) {
	d.distance = 0;
	d.stddev = dev;
	d.oldstddev = dev;
	d.avg = val;
	d.oldavg = val;
      } else {

        double sigma = d.stddev;
        if (sigma < 1) sigma = 1;
        d.distance = dev / sigma;
        //fprintf(stderr, "%g is %g sigma from avg of %g (sigma=%g)\n", val, d.distance, d.avg, d.stddev);
        d.distance *= d.distance;	// Square it

        d.oldavg = d.avg;
        d.oldstddev = d.stddev;

        if (val > d.avg) {
	  d.avg *= alpha_avg_up;
	  d.avg += (1-alpha_avg_up) * val;
        } else {
	  d.avg *= alpha_avg_down;
	  d.avg += (1-alpha_avg_down) * val;

	  // If asymmetric mode, only count distance above avg
	  if (alpha_avg_up != alpha_avg_down) {
	    d.distance = 0;
	  }
        }

        d.stddev *= alpha_stddev;
        d.stddev += (1-alpha_stddev) * dev;
      }

      total_distance += d.distance ;
      Dim[i].id.put(id, d);
    }
  }

  // Check for changes
  //fprintf(stderr, "total distance is %g\n", total_distance);
  total_distance = sqrt(total_distance);
  if (total_distance <= threshold) {
    //fprintf(stderr, "Skipping total distance = %g\n", total_distance);
    return SMACQ_FREE;
  }
		
  datum->attach_field(severity_field, 
		      dts->construct(double_type, &total_distance));

  /* We do this after the severity test since we hope that filters out
     most of the objects.  Otherwise, it would be faster to do it in the
     earlier dimension loop */
  if (attach_all) {
    for (int i = 0; i < dimensions; i++) {
        DtsObject raw = datum->getfield(Dim[i].raw_field);
	per_id_dimension_state d = Dim[i].id.get(id);
	assert(raw);
	raw->attach_field(avg_field,   dts->construct(double_type, &d.oldavg));
	raw->attach_field(sigma_field, dts->construct(double_type, &d.oldstddev));
    }
  }

  return SMACQ_PASS;
}

changesModule::changesModule(struct SmacqModule::smacq_init * context) 
  : SmacqModule(context)
{
  int argc;
  char ** argv;
  smacq_opt opt_threshold, opt_severity, opt_id, opt_alpha_avg_up, opt_alpha_avg_down, opt_alpha_stddev, opt_all, opt_base, opt_file;
    
  struct smacq_optval optvals[] = {
    { "histup", &opt_alpha_avg_up}, 
    { "histdown", &opt_alpha_avg_down}, 
    { "histdev", &opt_alpha_stddev}, 
    { "i", &opt_id}, 
    { "f", &opt_severity}, 
    { "t", &opt_threshold}, 
    { "a", &opt_all}, 
    { "base", &opt_base}, 
    { "s", &opt_file}, 
    {NULL, NULL}
  };
  smacq_getoptsbyname(context->argc-1, context->argv+1,
		      &argc, &argv,
		      options, optvals);

  attach_all = opt_all.boolean_t;
  if (attach_all) {
    avg_field = dts->requirefield("mean");
    sigma_field = dts->requirefield("sigma");
  }

  threshold = opt_threshold.double_t;
  severity_field = dts->requirefield(opt_severity.string_t);

  alpha_avg_up = opt_alpha_avg_up.double_t;
  alpha_avg_down = opt_alpha_avg_down.double_t;
  alpha_stddev = opt_alpha_stddev.double_t;

  base = opt_base.double_t;

  assert(alpha_avg_up <= 1); assert(alpha_avg_up >= 0);
  assert(alpha_avg_down <= 1); assert(alpha_avg_down >= 0);
  assert(alpha_stddev <= 1); assert(alpha_stddev >= 0);

  dimensions = argc;
  if (dimensions < 1) {
    fprintf(stderr, "One or more fields must be specified\n");
    assert(0);
  }
  for (int i = 0; i < argc; i++) {
    per_dimension_state d(dts, argv[i], opt_file.string_t);  
    Dim.push_back(d);
  }

  if (! opt_id.string_t) {
    fprintf(stderr, "changes: -i is required\n");
    assert(0);
  }
  id_field = dts->requirefield(opt_id.string_t);
  double_type = dts->requiretype("double");
}

