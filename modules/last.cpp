#include <stdlib.h>
#include <assert.h>
#include <smacq.h>
#include <FieldVec.h>
#include <dts.h>
#include <sys/time.h>

static struct smacq_options options[] = {
  {"t", {double_t:0}, "Update interval", SMACQ_OPT_TYPE_TIMEVAL},
  END_SMACQ_OPTIONS
};

SMACQ_MODULE(last, 
  PROTO_CTOR(last);
  PROTO_CONSUME();
  PROTO_PRODUCE();

	     FieldVec fieldvec;
	     FieldVecHash<DtsObject> last;
	     
	     struct timeval interval;
	     struct timeval nextinterval;
	     bool isstarted;
	     bool hasinterval;
	     
	     dts_field timeseries; // Field number
	     int refreshtype;
	     int timevaltype;

	     void emit_all();

); 

lastModule::lastModule(struct smacq_init * context) 
  : SmacqModule(context)
{
  int argc = 0;
  char ** argv;

  {
    smacq_opt interval_opt;
    
    struct smacq_optval optvals[] = {
      { "t", &interval_opt}, 
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

    interval = interval_opt.timeval_t;
    if ((interval_opt.timeval_t.tv_sec != 0) || 
	(interval_opt.timeval_t.tv_usec != 0)) {
      hasinterval = true;
      isstarted = false;
    } else {
      hasinterval = false;
    }
  }

  // Consume rest of arguments as fieldnames
  fieldvec.init(dts, argc, argv);

  timeseries = dts->requirefield("timeseries");
  refreshtype = dts->requiretype("refresh");
  timevaltype = dts->requiretype("timeval");
}

static inline void timeval_inc(struct timeval * x, struct timeval y) {
  x->tv_usec += y.tv_usec;
  x->tv_sec += y.tv_sec;

  if (x->tv_usec > 1000000) {
    x->tv_sec++;
    x->tv_usec -= 1000000;
  }
}

static inline int timeval_ge(struct timeval x, struct timeval y) {
  if (x.tv_sec > y.tv_sec) return 1;
  if ((x.tv_sec == y.tv_sec) && (x.tv_usec >= y.tv_usec)) return 1;
  return 0;
}
  
static inline int timeval_past(struct timeval x, struct timeval y) {
  if (x.tv_sec > y.tv_sec) return 1;
  if ((x.tv_sec == y.tv_sec) && (x.tv_usec > y.tv_usec)) return 1;
  return 0;
}

static inline void timeval_minus(struct timeval x, struct timeval y, struct timeval * result) {
  *result = x;
  x.tv_sec -= y.tv_sec; // No spec for underflow

  if (y.tv_usec > x.tv_usec) {
    x.tv_sec--;
    x.tv_usec += (int32_t)1e6;
  }
  x.tv_usec -= y.tv_usec;
  
  return;
}

void lastModule::emit_all() {
  FieldVecHash<DtsObject>::iterator i;
  
  for (i = last.begin(); i != last.end(); i++) {
    enqueue(i->second, 0);
  }

  // Last entry to be sent is a refresh message:
  if (fieldvec.empty()) {
	DtsObject obj = dts->construct(refreshtype, NULL);

  	if (hasinterval) {
  		DtsObject timefield;
  		timefield = dts->construct(timevaltype, &nextinterval);
  		obj->attach_field(timeseries, timefield);
  	}

  	enqueue(obj, 0);
	//fprintf(stderr, "last enqueue refresh %p\n", obj);
  }

}
  
smacq_result lastModule::consume(DtsObject datum, int & outchan) {
  smacq_result condproduce = (smacq_result)0;
  
  if (hasinterval) {
    DtsObject field_data;

    if (!(field_data = datum->getfield(timeseries))) {
      fprintf(stderr, "error: timeseries not available\n");
    } else {
      struct timeval * tv = (struct timeval *)field_data->getdata();
      assert(field_data->getsize() == sizeof(struct timeval));
      
      if (!isstarted) {
	isstarted = true;
	nextinterval = *tv;
	timeval_inc(&nextinterval, interval);
      } else if (timeval_ge(*tv, nextinterval)) {
	// Print counters
	emit_all();

	timeval_inc(&nextinterval, interval);
	while (timeval_past(*tv, nextinterval)) { // gap in timeseries
	  timeval_inc(&nextinterval, interval);
	}
      }
    }
  }

  fieldvec.getfields(datum);

  DtsObject old = last[fieldvec];
  
  last[fieldvec] = datum;

  return (SMACQ_FREE|canproduce());
}

smacq_result lastModule::produce(DtsObject & datum, int & outchan) {
  if (!canproduce()) {
    emit_all();
  }

  return dequeue(datum, outchan);
}
