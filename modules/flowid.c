/*
 * This module assigns a flow-id number to objects based on a tuple of
 * fields.  Optionally, flows can be made to timeout after an idle
 * time.  In this case, an end-of-flow record will be output.
 *
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

  struct smacq_outputq * outputq;

  struct timeval edge;

  // Internal stats
  int active;

  // Last flowid issued
  int flowid;

  // Cache
  int id_type;
  dts_field flowid_field;
  int refresh_type;
  dts_field start_field;
  dts_field finish_field;
  int timeval_type;
  dts_field ts_field; // Field number
  dts_field len_field;
  int len_type;
  dts_field byte_count_field;
  dts_field byte_count_back_field;
  dts_field packet_count_field;
  dts_field packet_count_back_field;
  
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

  msgdata = smacq_dts_construct(state->env, state->timeval_type, &s->starttime);
  dts_attach_field(datum, state->start_field, msgdata);
  
  msgdata = smacq_dts_construct(state->env, state->len_type, &s->byte_count);
  dts_attach_field(datum, state->byte_count_field, msgdata);
  
  msgdata = smacq_dts_construct(state->env, state->len_type, &s->byte_count_back);
  dts_attach_field(datum, state->byte_count_back_field, msgdata);
  
  msgdata = smacq_dts_construct(state->env, state->len_type, &s->packet_count);
  dts_attach_field(datum, state->packet_count_field, msgdata);
  
  msgdata = smacq_dts_construct(state->env, state->len_type, &s->packet_count_back);
  dts_attach_field(datum, state->packet_count_back_field, msgdata);

}

static inline int output(struct state * state, struct iovec * domainv, struct srcstat * s) {
    int i;

    // Output refresh record
      dts_object * msgdata;

      dts_object * refresh = smacq_dts_construct(state->env, state->refresh_type, NULL);

      msgdata = smacq_dts_construct(state->env, state->id_type, &s->id);
      dts_attach_field(refresh, state->flowid_field, msgdata);

      smacq_produce_enqueue(&state->outputq, refresh, -1);

      for (i = 0; i<state->fieldset.num; i++) {
	dts_attach_field(refresh, state->fieldset.fields[i].num, s->fields[i]);
      }

      msgdata = smacq_dts_construct(state->env, state->timeval_type, &s->lasttime);
      dts_attach_field(refresh, state->finish_field, msgdata);

      dts_attach_field(refresh, state->ts_field, msgdata);
      dts_incref(msgdata, 1);
  
      attach_stats(state, s, refresh);


    return 1;
}

static int expired(struct state * state, struct iovec * domainv, struct srcstat * s) {
  if (!state->hasinterval) return 0;

  if (s->expired) {
    assert(0); //Shouldn't happen
    return 1;
  }

  if (!timeval_past(s->lasttime, state->edge)) {
    s->expired = 1;

    output(state, domainv, s);

    // Cleanup
    free(s->fields);

    if (domainv)
    	bytes_hash_table_removev(state->stats, domainv, state->fieldset.num);

    return 1;
  }
  
  return 0;
}

static void output_all(gpointer key, gpointer val, gpointer user_data) {
  struct srcstat * s = val;
  struct state * state = user_data;

  output(state, NULL, s);
}

static inline int test_expired(gpointer key, gpointer val, gpointer user_data) {
  struct srcstat * s = val;
  struct state * state = user_data;
  
  return expired(state, NULL, s);
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

  const dts_object * field;
  dts_object * msgdata;
  struct timeval * tsnow;
  int size, swapped;

  // Get current time
  if (! (field = smacq_getfield(state->env, datum, state->ts_field, NULL))) {
    fprintf(stderr, "error: timeseries not available\n");
    return SMACQ_PASS;
  } else {
    tsnow = (struct timeval*) dts_getdata(field);
    dts_decref(field);
  }

  // Get current size
  if (! (field = smacq_getfield(state->env, datum, state->len_field, NULL))) {
    fprintf(stderr, "error: len not available\n");
    return SMACQ_PASS;
  } else {
    size = dts_data_as(field, unsigned int);
    dts_decref(field);
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
	s->fields[i] = smacq_getfield_copy(state->env, datum, state->fieldset.fields[i].num, NULL);
	assert(s->fields[i]);
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
    msgdata = smacq_dts_construct(state->env, state->id_type, &s->id);
    dts_attach_field(datum, state->flowid_field, msgdata);
    attach_stats(state, s, datum);
  }
 

  /* XXX: Very inefficient */
  if (state->hasinterval)
    bytes_hash_table_foreach_remove(state->stats, test_expired, state);

  if (state->outputq)
      status |= SMACQ_PRODUCE;

  //fprintf(stderr, "Expires list length %d\n", g_list_length(state->expires));
  return status;
}

static int flowid_init(struct smacq_init * context) {
  int argc = 0;
  char ** argv;
  struct state * state = context->state = calloc(sizeof(struct state), 1);
  state->env = context->env;

  state->refresh_type = smacq_requiretype(state->env, "refresh");

  state->flowid_field = smacq_requirefield(state->env, "flowid");
  state->id_type = smacq_requiretype(state->env, "int");

  state->len_type = smacq_requiretype(state->env, "uint32");

  state->byte_count_field = smacq_requirefield(state->env, "bytes");
  state->byte_count_back_field = smacq_requirefield(state->env, "bytesback");

  state->packet_count_field = smacq_requirefield(state->env, "packets");
  state->packet_count_back_field = smacq_requirefield(state->env, "packetsback");

  state->ts_field = smacq_requirefield(state->env, "timeseries");
  state->timeval_type = smacq_requiretype(state->env, "timeval");

  state->start_field = smacq_requirefield(state->env, "start");
  state->finish_field = smacq_requirefield(state->env, "finish");
  state->len_field = smacq_requirefield(state->env, "len");

  {
	smacq_opt interval, reverse;

  	struct smacq_optval optvals[] = {
    		{ "t", &interval}, 
    		{ "r", &reverse}, 
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
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

  return 0;
}

static int flowid_shutdown(struct state * state) {
  free(state);
  return SMACQ_END;
}


static smacq_result flowid_produce(struct state * state, const dts_object ** datum, int * outchan) {
  if (state->outputq) {
    return smacq_produce_dequeue(&state->outputq, datum, outchan);
  } else {
    //fprintf(stderr, "flowid: produce called with nothing in queue.  Outputing everything in current table!\n");

    bytes_hash_table_foreach(state->stats, output_all, state);

    if (!state->outputq)
	    return SMACQ_FREE;

    return flowid_produce(state, datum, outchan);
  }

}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_flowid_table = {
  &flowid_produce, 
  &flowid_consume,
  &flowid_init,
  &flowid_shutdown
};

