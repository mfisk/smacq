#include <math.h>
#include <SmacqModule.h>
#include <DynamicArray.h>
#include <FieldVec.h>

static struct smacq_options options[] = {
  {"histup", {double_t:0}, "History when moving average up", SMACQ_OPT_TYPE_DOUBLE},
  {"histdown", {double_t:0.9995}, "History when moving average down", SMACQ_OPT_TYPE_DOUBLE},
  {"histdev", {double_t:0.8}, "Standard Deviation history", SMACQ_OPT_TYPE_DOUBLE},
  {"f", {string_t:"severity"}, "Report severity of change as this field", SMACQ_OPT_TYPE_STRING},
  {"t", {double_t:-1}, "Threshold (in standard deviations)", SMACQ_OPT_TYPE_DOUBLE},
  {"i", {string_t:NULL}, "Identify field", SMACQ_OPT_TYPE_STRING},
  {"a", {boolean_t:0}, "Add mean and sigma fields to each feature", SMACQ_OPT_TYPE_BOOLEAN},
  {"base", {double_t:1}, "Feature value is log with this base", SMACQ_OPT_TYPE_DOUBLE},
  END_SMACQ_OPTIONS
};

class per_id_dimension_state {
public:
  per_id_dimension_state() : avg(0), stddev(0), distance(0) {}
  
  double avg;
  double stddev;
  double distance;

  double oldavg;
  double oldstddev;

  DtsObject last_field;
};

class per_dimension_state {
public:
  DtsField  field, raw_field;
};

class per_id_state {
public:
  DynamicArray<struct per_id_dimension_state> dim;
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

	     FieldVecHash<per_id_state> Id;
	     std::vector<per_dimension_state> Dim;
	     );

smacq_result changesModule::consume(DtsObject datum, int & outchan) {
  double total_distance = 0;

  DtsObject id = datum->getfield(id_field);
  if (!id) {
    //fprintf(stderr, "ID field not present\n");
    return SMACQ_FREE;
  }
  per_id_state & s = Id[id];

  for (int i = 0; i < dimensions; i++) {
    per_id_dimension_state & d = s.dim[i];
    DtsObject o = datum->getfield(Dim[i].field);
		
    // make sure we got it
    if (o) {
      d.last_field = datum->getfield(Dim[i].raw_field);
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
	continue;
      }

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

  if (attach_all) {
  	for (int i = 0; i < dimensions; i++) {
    		per_id_dimension_state & d = s.dim[i];
    		if (!datum->getfield(Dim[i].raw_field)) {
			datum->attach_field(Dim[i].raw_field, d.last_field);
		}
	
		//fprintf(stderr, "attaching to %p field of %p\n", o.get(), datum.get());
		if (d.last_field) {
		   d.last_field->attach_field(avg_field, 
			   dts->construct(double_type, &d.oldavg));

		   d.last_field->attach_field(sigma_field, 
			   dts->construct(double_type, &d.oldstddev));
		}
	}
    }

  return SMACQ_PASS;
}

changesModule::changesModule(smacq_init * context) 
  : SmacqModule(context)
{
  int argc;
  char ** argv;
  smacq_opt opt_threshold, opt_severity, opt_id, opt_alpha_avg_up, opt_alpha_avg_down, opt_alpha_stddev, opt_all, opt_base;
    
  struct smacq_optval optvals[] = {
    { "histup", &opt_alpha_avg_up}, 
    { "histdown", &opt_alpha_avg_down}, 
    { "histdev", &opt_alpha_stddev}, 
    { "i", &opt_id}, 
    { "f", &opt_severity}, 
    { "t", &opt_threshold}, 
    { "a", &opt_all}, 
    { "base", &opt_base}, 
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
  Dim.resize(dimensions);
  for (int i = 0; i < argc; i++) {
    Dim[i].field = dts->requirefield(dts_fieldname_append(argv[i], "double"));
    Dim[i].raw_field = dts->requirefield(argv[i]);
  }

  if (! opt_id.string_t) {
    fprintf(stderr, "changes: -i is required\n");
    assert(0);
  }
  id_field = dts->requirefield(opt_id.string_t);
  double_type = dts->requiretype("double");

}

