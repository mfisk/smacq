#include <stdlib.h>
#include <assert.h>
#include <smacq.h>

/* Programming constants */

static struct smacq_options options[] = {
  {"t", {double_t:0}, "Lag time", SMACQ_OPT_TYPE_TIMEVAL},
  {"i", {string_t:"timeseries"}, "Time field used to update time", SMACQ_OPT_TYPE_STRING},
  {"o", {string_t:"timeseries"}, "Time field to gate output data", SMACQ_OPT_TYPE_STRING},
  END_SMACQ_OPTIONS,
};

struct obj_list{
  DtsObject obj;
  struct obj_list * next;
  struct timeval ts;
};

SMACQ_MODULE(fifodelay,
  PROTO_CTOR(fifodelay);
  PROTO_CONSUME();
  PROTO_PRODUCE();

  struct obj_list * fifo;
  struct obj_list * last;
  dts_field ts_field;
  dts_field edge_field;
  struct timeval edge;
  struct timeval interval;

  struct obj_list * fifo_insert(DtsObject datum, struct timeval ts);
);

static void timeval_minus(struct timeval x, struct timeval y, struct timeval * result) {
  *result = x;
  result->tv_sec -= y.tv_sec; // No spec for underflow

  if (y.tv_usec > result->tv_usec) {
    result->tv_sec--;
    result->tv_usec += (int32_t)1e6;
  }
  result->tv_usec -= y.tv_usec;
  
  return;
}

static int timeval_past(struct timeval x, struct timeval y) {
  if (x.tv_sec > y.tv_sec) return 1;
  if ((x.tv_sec == y.tv_sec) && (x.tv_usec > y.tv_usec)) return 1;
  return 0;
}

struct obj_list * fifodelayModule::fifo_insert(DtsObject datum, struct timeval ts) {
  struct obj_list * newo = g_new(struct obj_list, 1);
  struct obj_list * i;
  newo->obj = (DtsObject)datum;
  newo->next = NULL;
  newo->ts = ts;

  if (fifo) {
    if (timeval_past(fifo->ts, ts)) {
      newo->next = fifo;
      fifo = newo;

      //fprintf(stderr, "%d inserted at head before %d\n", ts.tv_sec, newo->next->ts.tv_sec);
      
    } else {

      for (i = fifo; i->next; i=i->next) {
	if (timeval_past(i->next->ts, ts)) break;
      }
      
      newo->next = i->next;
      i->next = newo;

      //fprintf(stderr, "%d inserted between %d and %d\n", ts.tv_sec, i->ts.tv_sec, newo->next ? newo->next->ts.tv_sec : 0);
    }
      
    if (!newo->next) {
      last = newo;
    }
  } else {
    //fprintf(stderr, "%d inserted as only element\n", ts.tv_sec);
    fifo = newo;
    last = newo;
 }

  

  return newo;
}

smacq_result fifodelayModule::consume(DtsObject datum, int * outchan) {
  DtsObject dtime;

  /* Add this entry to the queue */
  dtime = datum->getfield(ts_field);
  if (!dtime) {
    fprintf(stderr, "Passing field without time\n");
    return SMACQ_PASS;
  }
  assert(dtime->getsize() == sizeof (struct timeval));

  fifo_insert(datum,  dts_data_as(dtime, struct timeval));
  
  /* Update the edge time, if given in this packet */
  dtime = datum->getfield(edge_field);
  if (dtime) {
    assert(dtime->getsize() == sizeof (struct timeval));
    timeval_minus( dts_data_as(dtime, struct timeval), interval, &edge);
  }
  return (smacq_result)(SMACQ_FREE|SMACQ_PRODUCE);
}

fifodelayModule::fifodelayModule(struct smacq_init * context) : SmacqModule(context) {
  int argc = 0;
  char ** argv;

  {
    smacq_opt intervalo, ifieldname, ofieldname;

    struct smacq_optval optvals[] = {
      {"t", &intervalo},
      {"i", &ifieldname},
      {"o", &ofieldname},
      {NULL, NULL},
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
		       &argc, &argv,
		       options, optvals);

    interval = intervalo.timeval_t;
    edge_field = dts->requirefield(ifieldname.string_t);
    ts_field = dts->requirefield(ofieldname.string_t);
    assert(ts_field);

    assert(argc==0);
  }
}

smacq_result fifodelayModule::produce(DtsObject & datum, int * outchan) {
  if (fifo) {
    struct obj_list * old = fifo;

    if (timeval_past(old->ts, edge)) {
      //fprintf(stderr, "%d waiting for %d:\t %d left\n", edge.tv_sec, old->ts.tv_sec, old->ts.tv_sec  - edge.tv_sec);
      return SMACQ_FREE;
    } else {
      //fprintf(stderr, "producing\n");
    }

    if (last == fifo) {
      last = NULL;
    }
    fifo = fifo->next;

    datum = old->obj;
    free(old);

    
    return (smacq_result)(SMACQ_PASS|(fifo ? SMACQ_PRODUCE : 0));
  } else {
    datum = NULL;
    return(SMACQ_FREE);
  }
}
