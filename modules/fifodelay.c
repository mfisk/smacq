#include <stdlib.h>
#include <assert.h>
#include <smacq.h>

/* Programming constants */

static struct smacq_options options[] = {
  {"t", {double_t:0}, "Lag time", SMACQ_OPT_TYPE_TIMEVAL},
  {"i", {string_t:"timeseries"}, "Time field used to update time", SMACQ_OPT_TYPE_STRING},
  {"o", {string_t:"timeseries"}, "Time field to gate output data", SMACQ_OPT_TYPE_STRING},
  {NULL, {string_t:NULL}, NULL, 0},
};

struct obj_list{
  dts_object * obj;
  struct obj_list * next;
  struct timeval ts;
};

struct state {
  smacq_environment * env;
  struct obj_list * fifo, * last;
  int ts_field, edge_field;
  struct timeval edge, interval;
}; 

static void timeval_minus(struct timeval x, struct timeval y, struct timeval * result) {
  *result = x;
  result->tv_sec -= y.tv_sec; // No spec for underflow

  if (y.tv_usec > result->tv_usec) {
    result->tv_sec--;
    result->tv_usec += 1e6;
  }
  result->tv_usec -= y.tv_usec;
  
  return;
}

static int timeval_past(struct timeval x, struct timeval y) {
  if (x.tv_sec > y.tv_sec) return 1;
  if ((x.tv_sec == y.tv_sec) && (x.tv_usec > y.tv_usec)) return 1;
  return 0;
}

struct obj_list * fifo_insert(struct state * state, const dts_object * datum, struct timeval ts) {
  struct obj_list * newo = g_new(struct obj_list, 1);
  struct obj_list * i;
  newo->obj = (dts_object*)datum;
  newo->next = NULL;
  newo->ts = ts;

  if (state->fifo) {
    if (timeval_past(state->fifo->ts, ts)) {
      newo->next = state->fifo;
      state->fifo = newo;

      //fprintf(stderr, "%d inserted at head before %d\n", ts.tv_sec, newo->next->ts.tv_sec);
      
    } else {

      for (i = state->fifo; i->next; i=i->next) {
	if (timeval_past(i->next->ts, ts)) break;
      }
      
      newo->next = i->next;
      i->next = newo;

      //fprintf(stderr, "%d inserted between %d and %d\n", ts.tv_sec, i->ts.tv_sec, newo->next ? newo->next->ts.tv_sec : 0);
    }
      
    if (!newo->next) {
      state->last = newo;
    }
  } else {
    //fprintf(stderr, "%d inserted as only element\n", ts.tv_sec);
    state->fifo = newo;
    state->last = newo;
 }

  dts_incref(datum, 1);

  return newo;
}

static smacq_result fifodelay_consume(struct state * state, const dts_object * datum, int * outchan) {
  dts_object dtime;
  int res;

  /* Add this entry to the queue */
  res = flow_getfield(state->env, datum, state->ts_field, &dtime);
  if (!res) {
    fprintf(stderr, "Passing field without time\n");
    return SMACQ_PASS;
  }
  assert(dtime.len == sizeof (struct timeval));

  fifo_insert(state, datum,  *(struct timeval *)dtime.data);

  /* Update the edge time, if given in this packet */
  res = flow_getfield(state->env, datum, state->edge_field, &dtime);
  if (res) {
    assert(dtime.len == sizeof (struct timeval));
    timeval_minus(  *(struct timeval*)dtime.data, state->interval, &state->edge);
  }
  return(SMACQ_FREE|SMACQ_PRODUCE);
}

static int fifodelay_init(struct flow_init * context) {
  int argc = 0;
  char ** argv;
  struct state * state = context->state = g_new0(struct state, 1);
  state->env = context->env;
  


  {
    smacq_opt interval, ifieldname, ofieldname;

    struct smacq_optval optvals[] = {
      {"t", &interval},
      {"i", &ifieldname},
      {"o", &ofieldname},
      {NULL, NULL},
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
		       &argc, &argv,
		       options, optvals);

    state->interval = interval.timeval_t;
    state->edge_field = flow_requirefield(state->env, ifieldname.string_t);
    state->ts_field = flow_requirefield(state->env, ofieldname.string_t);
    assert(state->ts_field);

    assert(argc==0);
  }

  return 0;
}

static int fifodelay_shutdown(struct state * state) {
  return 0;
}

static smacq_result fifodelay_produce(struct state * state, const dts_object ** datum, int * outchan) {
  if (state->fifo) {
    struct obj_list * old = state->fifo;

    if (timeval_past(old->ts, state->edge)) {
      //fprintf(stderr, "%d waiting for %d:\t %d left\n", state->edge.tv_sec, old->ts.tv_sec, old->ts.tv_sec  - state->edge.tv_sec);
      return SMACQ_FREE;
    } else {
      //fprintf(stderr, "producing\n");
    }

    if (state->last == state->fifo) {
      state->last = NULL;
    }
    state->fifo = state->fifo->next;
    

    *datum = old->obj;
    free(old);

    dts_incref(*datum, 1);
    return(SMACQ_PASS|(state->fifo ? SMACQ_PRODUCE : 0));
  } else {
    *datum = NULL;
    return(SMACQ_FREE);
  }
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_fifodelay_table = {
  &fifodelay_produce, 
  &fifodelay_consume,
  &fifodelay_init,
  &fifodelay_shutdown
};
