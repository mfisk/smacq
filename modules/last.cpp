#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
#include <fields.h>
#include <bytehash.h>
#include <dts.h>
#include <produceq.h>
#include <sys/time.h>

/* Programming constants */
#define KEYBYTES 128

static struct smacq_options options[] = {
  {"t", {double_t:0}, "Update interval", SMACQ_OPT_TYPE_TIMEVAL},
  {NULL, {string_t:NULL}, NULL, 0}
};

SMACQ_MODULE(last, 
  PROTO_CTOR(last);
  PROTO_CONSUME();
  PROTO_PRODUCE();

	     struct smacq_outputq * outputq;
	     struct fieldset fieldset;
	     struct iovec_hash *last;
	     
	     struct timeval interval;
	     struct timeval nextinterval;
	     int istarted;
	     int hasinterval;
	     
	     dts_field timeseries; // Field number
	     int refreshtype;
	     int timevaltype;

	     void emit_all();

	static int emit_last(struct element * key, void * value, void * userdata);
); 

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
    x.tv_usec += 1e6;
  }
  x.tv_usec -= y.tv_usec;
  
  return;
}

static int lastModule::emit_last(struct element * key, void * value, void * userdata) {
  DtsObject * d = value;
  lastModule * l = (lastModule*)userdata;

  fprintf(stderr, "emit_last on obj %p\n", value);

  smacq_produce_enqueue(&l->outputq, d, -1);
  d->incref();

  return 0;
}
	
void lastModule::emit_all() {
  assert (!outputq);
  bytes_hash_table_foreach(last, emit_last, this);

  // Last entry to be sent is a refresh message:
  if (fieldset.num) {
	DtsObject * obj = dts->construct(refreshtype, NULL);

  	if (hasinterval) {
  		DtsObject * timefield;
  		timefield = dts->construct(timevaltype, &nextinterval);
  		obj->attach_field(timeseries, timefield);
  	}

  	smacq_produce_enqueue(&outputq, obj, -1);
	//fprintf(stderr, "last enqueue refresh %p\n", obj);
  }

}
  
smacq_result lastModule::consume(DtsObject * datum, int * outchan) {
  struct iovec * domainv = datum->fields2vec(&fieldset);
  int condproduce = 0;

  if (hasinterval) {
    DtsObject * field_data;

    if (!(field_data = datum->getfield(timeseries))) {
      fprintf(stderr, "error: timeseries not available\n");
    } else {
      struct timeval * tv = (struct timeval *)field_data->getdata();
      assert(field_data->getsize() == sizeof(struct timeval));
      
      if (!istarted) {
	istarted = 1;
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

  if (outputq) {
    condproduce = SMACQ_PRODUCE;
  }

  if (!domainv && fieldset.num) {
      //fprintf(stderr, "Skipping datum\n");
      return SMACQ_PASS|condproduce;
  }

  {
	DtsObject * old; 

  	datum->incref();
  	old = bytes_hash_table_setv(last, domainv, fieldset.num, (gpointer)datum);
	//fprintf(stderr, "last saving %p, releasing %p\n", datum, old);
	if (old) 
	  old->decref();
  }

  return(SMACQ_FREE|condproduce);
}

lastModule::lastModule(struct smacq_init * context) : SmacqModule(context) {
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
      hasinterval = 1;
    } else {
      hasinterval = 0;
    }
  }

  // Consume rest of arguments as fieldnames
  dts->fields_init(&fieldset, argc, argv);

  timeseries = dts->requirefield("timeseries");
  refreshtype = dts->requiretype("refresh");
  timevaltype = dts->requiretype("timeval");
  last = bytes_hash_table_new(KEYBYTES, CHAIN|NOFREE);
}

smacq_result lastModule::produce(DtsObject ** datum, int * outchan) {
  if (!outputq) {
    emit_all();
  }

  return smacq_produce_dequeue(&outputq, datum, outchan);
}
