/*
 * This module assigns a flow-id number to objects based on a tuple of
 * fields.  Optionally, flows can be made to timeout after an idle
 * time.  In this case, an end-of-flow record will be output.
 *
*/

/* 
 * XXX: Never deletes things from hash table.  Uses horribly inefficient list operations.
 * Need to use different data structures.
 */

#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
#include <dts_packet.h>
#include <fields.h>
#include "bytehash.h"

/* Programming constants */

#define VECTORSIZE 21
#define ALARM_BITS 15
#define KEYBYTES 128

static struct smacq_options options[] = {
  {"t", {double_t:0}, "Threshold quiet time", SMACQ_OPT_TYPE_TIMEVAL},
  {"r", {boolean_t:0}, "Reverse each pair of fields for bidirectional flows", SMACQ_OPT_TYPE_BOOLEAN},
  {NULL, {string_t:NULL}, NULL, 0}
};

struct srcstat {
  struct timeval starttime, lasttime;
  int expired;
  int id;

  unsigned int byte_count, byte_count_back;
  unsigned int packet_count, packet_count_back;

  const dts_object ** fields;
};

struct state {
  smacq_environment * env;
  struct fieldset fieldset, fieldset2;
  GHashTableofBytes *stats;

  struct timeval interval;
  struct timeval nextgc;
  int hasinterval;

  GQueue* outputq;

  struct timeval edge;

  // Internal stats
  int active;

  // Last flowid issued
  int flowid;

  // Cache
  int id_type;
  int flowid_field;
  int refresh_type;
  int start_field;
  int finish_field;
  int timeval_type;
  int ts_field; // Field number
  int len_field;
  int len_type;
  int byte_count_field;
  int byte_count_back_field;
  int packet_count_field;
  int packet_count_back_field;
  
  int reverse;
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

void attach_stats(struct state * state, struct srcstat * s, const dts_object * datum) {
  dts_object * msgdata;

  msgdata = flow_dts_construct(state->env, state->timeval_type, &s->starttime);
  dts_attach_field(datum, state->start_field, msgdata);
  
  msgdata = flow_dts_construct(state->env, state->len_type, &s->byte_count);
  dts_attach_field(datum, state->byte_count_field, msgdata);
  
  msgdata = flow_dts_construct(state->env, state->len_type, &s->byte_count_back);
  dts_attach_field(datum, state->byte_count_back_field, msgdata);
  
  msgdata = flow_dts_construct(state->env, state->len_type, &s->packet_count);
  dts_attach_field(datum, state->packet_count_field, msgdata);
  
  msgdata = flow_dts_construct(state->env, state->len_type, &s->packet_count_back);
  dts_attach_field(datum, state->packet_count_back_field, msgdata);

}

int expired(struct state * state, struct iovec * domainv, struct srcstat * s) {
  int i;
  if (!state->hasinterval) return 0;

  if (s->expired) {
    //assert(0); //Shouldn't happen
    return 1;
  }

  if (!timeval_past(s->lasttime, state->edge)) {
    s->expired = 1 ;
    
    // Output refresh record
    {
      dts_object * msgdata;

      dts_object * refresh = flow_dts_construct(state->env, state->refresh_type, NULL);

      msgdata = flow_dts_construct(state->env, state->id_type, &s->id);
      dts_attach_field(refresh, state->flowid_field, msgdata);

      g_queue_push_tail(state->outputq, refresh);

      for (i = 0; i<state->fieldset.num; i++) {
	dts_attach_field(refresh, state->fieldset.fields[i].num, s->fields[i]);
	dts_incref(s->fields[i], 1);
      }

      msgdata = flow_dts_construct(state->env, state->timeval_type, &s->lasttime);
      dts_attach_field(refresh, state->finish_field, msgdata);

      dts_attach_field(refresh, state->ts_field, msgdata);
      dts_incref(msgdata, 1);
  
      attach_stats(state, s, refresh);
    }

    // Cealnup
    for (i = 0; i<state->fieldset.num; i++) {
	dts_decref(s->fields[i]);
    }
    free(s->fields);

    if (domainv) 
      bytes_hash_table_removev(state->stats, domainv, state->fieldset.num);

    return 1;
  }

  
  return 0;
}

static inline int test_expired(gpointer key, gpointer val, gpointer user_data) {
  struct srcstat * s = val;
  struct state * state = user_data;
  
  expired(state, NULL, s);
  return 0;
}

  /*
 * Plan of attack: 
 *
 * 1) Find out what time it is now.
 * 2) See if the current object belongs to an active flow, else create one
 * 3) Timout idle flows and send out a final record
 *
 */
static smacq_result flowid_consume(struct state * state, const dts_object * datum, int * outchan) {
  struct iovec * domainv = NULL;
  struct srcstat * s;
  int status = SMACQ_FREE;

  dts_object field;
  dts_object * msgdata;
  struct timeval * tsnow;
  int size, swapped;

  // Get current time
  if (!flow_getfield(state->env, datum, state->ts_field, &field)) {
    fprintf(stderr, "error: timeseries not available\n");
    return SMACQ_PASS;
  } else {
    tsnow = (struct timeval*)field.data;
    assert(field.len == sizeof(struct timeval));
  }

  // Get current size
  if (!flow_getfield(state->env, datum, state->len_field, &field)) {
    fprintf(stderr, "error: len not available\n");
    return SMACQ_PASS;
  } else {
    size = *(unsigned int*)field.data;
    assert(field.len == sizeof(unsigned int));
  }

  // Find this entry
  domainv = fields2vec(state->env, datum, &state->fieldset);

  if (!domainv) {
    //fprintf(stderr, "Skipping datum\n");
    return SMACQ_FREE;
  }
  s = bytes_hash_table_lookupv(state->stats, domainv, state->fieldset.num);
 
  swapped = 0;

  if (state->hasinterval) 
    timeval_minus(*tsnow, state->interval, &state->edge);

  // Make new entry if necessary 
  if (!s || expired(state, domainv, s)) {
    if (state->reverse) {
      struct iovec * domain2v =  fields2vec(state->env, datum, &state->fieldset2);
      s = bytes_hash_table_lookupv(state->stats, domain2v, state->fieldset2.num);
    }

    if (!s || expired(state, domainv, s)) {
      int i;

      s = g_new0(struct srcstat, 1);
      s->id = state->flowid++;
      s->starttime = *tsnow;
      s->fields = g_new(const dts_object*, state->fieldset.num);
      for (i = 0; i<state->fieldset.num; i++) {
	int res;
	s->fields[i] = flow_alloc(state->env, 0, 0);
	res = flow_getfield_copy(state->env, datum, state->fieldset.fields[i].num, (dts_object*)s->fields[i]);
	assert (res);
	assert(s->fields[i]);
	dts_incref(s->fields[i], 1);
      }
	
      bytes_hash_table_replacev(state->stats, domainv, state->fieldset.num, s);
      state->active++;
      status = SMACQ_PASS;
    } else {
      swapped = 1;
    }
  }

  // Update state
  s->lasttime = *tsnow;
  if (swapped) {
    s->byte_count_back += size;
    s->packet_count_back ++;
  } else {
    s->byte_count += size;
    s->packet_count ++;
  }

  if (status & SMACQ_PASS) {
    // Attach flowid to this datum
    msgdata = flow_dts_construct(state->env, state->id_type, &s->id);
    dts_attach_field(datum, state->flowid_field, msgdata);
    attach_stats(state, s, datum);
  }
  
  if (state->hasinterval)
    bytes_hash_table_foreach_remove(state->stats, test_expired, state);

  if (! g_queue_is_empty(state->outputq) )
      status |= SMACQ_PRODUCE;

  //fprintf(stderr, "Expires list length %d\n", g_list_length(state->expires));
  return status;
}

static int flowid_init(struct flow_init * context) {
  int argc = 0;
  char ** argv;
  struct state * state = context->state = g_new0(struct state, 1);
  state->env = context->env;

  state->refresh_type = flow_requiretype(state->env, "refresh");

  state->flowid_field = flow_requirefield(state->env, "flowid");
  state->id_type = flow_requiretype(state->env, "int");

  state->len_type = flow_requiretype(state->env, "uint32");

  state->byte_count_field = flow_requirefield(state->env, "bytes");
  state->byte_count_back_field = flow_requirefield(state->env, "bytesback");

  state->packet_count_field = flow_requirefield(state->env, "packets");
  state->packet_count_back_field = flow_requirefield(state->env, "packetsback");

  state->ts_field = flow_requirefield(state->env, "timeseries");
  state->timeval_type = flow_requiretype(state->env, "timeval");

  state->start_field = flow_requirefield(state->env, "start");
  state->finish_field = flow_requirefield(state->env, "finish");
  state->len_field = flow_requirefield(state->env, "len");

  {
	smacq_opt interval, reverse;

  	struct smacq_optval optvals[] = {
    		{ "t", &interval}, 
    		{ "r", &reverse}, 
    		{NULL, NULL}
  	};
  	flow_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

	state->interval = interval.timeval_t;
	state->hasinterval = (state->interval.tv_usec || state->interval.tv_sec); 
	state->reverse = reverse.boolean_t;
  }

  // Consume rest of arguments as field names
  fields_init(state->env, &state->fieldset, argc, argv);

  if (state->reverse) {
    int i;
    char ** rargv = g_new(char*, argc);
    for (i = 0; i < argc; i++) {
      if (i %2) {
	assert(i > 0);
	rargv[i] = argv[i-1];
      } else {
	assert(i < (argc-1));
	rargv[i] = argv[i+1];
      }
    }
    fields_init(state->env, &state->fieldset2, argc, rargv);
    free(rargv);
  }

  state->stats = bytes_hash_table_new(KEYBYTES, CHAIN, FREE);

  state->outputq = g_queue_new();

  return 0;
}

static int flowid_shutdown(struct state * state) {
  g_queue_free(state->outputq);
  free(state);
  return SMACQ_END;
}


static smacq_result flowid_produce(struct state * state, const dts_object ** datum, int * outchan) {
  int status = 0;

  *datum = g_queue_pop_head(state->outputq);

  if (! g_queue_is_empty(state->outputq) )
      status |= SMACQ_PRODUCE;
    
  if (*datum) 
    status |= SMACQ_PASS;
  else
    status |= SMACQ_FREE;

  return status;
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_flowid_table = {
  &flowid_produce, 
  &flowid_consume,
  &flowid_init,
  &flowid_shutdown
};

