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
};

struct state {
  smacq_environment * env;
  struct fieldset fieldset, fieldset2;
  GHashTableofBytes *stats;

  struct timeval interval;
  int hasinterval;

  GList * expires;

  int timeseries; // Field number

  // Internal stats
  int active;

  // Last flowid issued
  int flowid;

  // Cache
  int id_type;
  int flowid_field;
  int refresh_type;
  
  // Output 
  dts_object * product;

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
  int status = SMACQ_PASS;

  dts_object field;
  dts_object * msgdata;
  struct timeval * tsnow;

  // Get current time
  if (!flow_getfield(state->env, datum, state->timeseries, &field)) {
    fprintf(stderr, "error: timeseries not available\n");
    return SMACQ_PASS;
  } else {
    tsnow = (struct timeval*)field.data;
    assert(field.len == sizeof(struct timeval));
  }


  // Manage expires list
  if (state->hasinterval) {
    struct timeval thold;
    GList * thisel;
    timeval_minus(*tsnow, state->interval, &thold);
    
    while((thisel = g_list_first(state->expires))) {
	struct srcstat * this = thisel->data;
	
	if (!this || timeval_past(this->lasttime, thold)) break; // Future stuff to expire
	state->active--;
	this->expired = 1;
	state->expires = g_list_delete_link(state->expires, thisel);
	
	// Output refresh record
	{
	  dts_object * refresh = flow_dts_construct(state->env, state->refresh_type, NULL);
	  dts_object * msgdata = flow_dts_construct(state->env, state->id_type, &this->id);
	  dts_attach_field(refresh, state->flowid_field, msgdata);
	  state->product = refresh;
	  status |= SMACQ_PRODUCE;
	}

	//bytes_hash_table_removev(state->stats, ...);
	//free(this);
	//free(thisel);
    }
  }

  // Find this entry
  domainv = fields2vec(state->env, datum, &state->fieldset);

  if (!domainv) {
    //fprintf(stderr, "Skipping datum\n");
    return SMACQ_FREE;
  }
  s = bytes_hash_table_lookupv(state->stats, domainv, state->fieldset.num);

  // Make new entry if necessary 
  if (!s || s->expired) {
    if (state->reverse) {
      struct iovec * domain2v =  fields2vec(state->env, datum, &state->fieldset2);
      s = bytes_hash_table_lookupv(state->stats, domain2v, state->fieldset2.num);
    }

    if (!s || s->expired) {
      s = g_new(struct srcstat, 1);
      s->expired = 0;
      s->id = state->flowid++;
      s->starttime = *tsnow;
      bytes_hash_table_insertv(state->stats, domainv, state->fieldset.num, s);
      state->active++;
    }
  }

  // Update state
  s->lasttime = *tsnow;

  // Attach flowid to this datum
  msgdata = flow_dts_construct(state->env, state->id_type, &s->id);
  dts_attach_field(datum, state->flowid_field, msgdata);

  // Update expires list (horribly expensive as implemented. XXX.)
  if (state->hasinterval) {
    state->expires = g_list_remove(state->expires, s);
    state->expires = g_list_append(state->expires, s);
  }


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

  state->timeseries = flow_requirefield(state->env, "timeseries");

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
  }

  state->stats = bytes_hash_table_new(KEYBYTES, CHAIN, FREE);

  return 0;
}

static int flowid_shutdown(struct state * state) {
  return SMACQ_END;
}


static smacq_result flowid_produce(struct state * state, const dts_object ** datum, int * outchan) {
  *datum = state->product;
  return SMACQ_PASS;
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_flowid_table = {
  &flowid_produce, 
  &flowid_consume,
  &flowid_init,
  &flowid_shutdown
};

