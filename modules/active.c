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
  {NULL, {string_t:NULL}, NULL, 0}
};

struct srcstat {
  struct timeval starttime, lasttime;
  int count;
  int inlist;
  int expired;
};

struct state {
  smacq_environment * env;
  struct fieldset fieldset;
  GHashTableofBytes *stats;

  FILE * printfd;

  struct srcstat unified;

  struct timeval interval;
  int hasinterval;

  GList * expires;
  int gc_count;

  int timeseries; // Field number
  int activetype, activefield;
  int active;
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

static int isexpired(gpointer key, gpointer value, gpointer userdata) {
	return ((struct srcstat*)value)->expired;
}

/*
 * Plan of attack: 
 *
 * 1) Find out what time it is now.
 * 2) See if the current object belongs to an active flow, else create one
 * 3) Timout idle flows and send out a final record
 *
 */
static smacq_result active_consume(struct state * state, const dts_object * datum, int * outchan) {
  struct iovec * domainv = NULL;
  struct srcstat * s;

  const dts_object * field;
  dts_object * activefielddata;
  struct timeval * value;

  // Get current time
  if (!(field = smacq_getfield(state->env, datum, state->timeseries, &NULL))) {
    fprintf(stderr, "error: timeseries not available\n");
    return SMACQ_PASS;
  } else {
    value = (struct timeval*)dts_getdata(field);
    assert(field.len == sizeof(struct timeval));
  }

  // Find this entry
  if (!state->fieldset.num) {
    s = &(state->unified);
  } else {
    domainv = fields2vec(state->env, datum, &state->fieldset);

    if (!domainv) {
      //fprintf(stderr, "Skipping datum\n");
      return SMACQ_FREE;
    }

    s = bytes_hash_table_lookupv(state->stats, domainv, state->fieldset.num);
  }

  // Manage expires list
  if (state->hasinterval) {
    state->expires = g_list_remove(state->expires, s);
    state->expires = g_list_append(state->expires, s);
    {
      struct timeval thold;
      GList * thisel;
      timeval_minus(*value, state->interval, &thold);
      
      while((thisel = g_list_first(state->expires))) {
	struct srcstat * this = thisel->data;

	if (!this || timeval_past(this->lasttime, thold)) break; // Future stuff to expire
	state->active--;
	this->expired = 1;
	state->expires = g_list_delete_link(state->expires, thisel);
      }
    }
  }

  // Garbage collect
  if (! --state->gc_count) {
  	bytes_hash_table_foreach_remove(state->stats, isexpired, NULL);
   	state->gc_count = 1000;
  }

  if (!s) {
    s = g_new(struct srcstat,1);
    bytes_hash_table_insertv(state->stats, domainv, state->fieldset.num, s);
    state->active++;
  } else if (s->expired) {
    s->expired = 0;
    state->active++;
  }
  s->lasttime = *value;

  // Attach active count to this datum
  activefielddata = smacq_dts_construct(state->env, state->activetype, &state->active);
  dts_attach_field(datum, state->activefield, activefielddata);

  //fprintf(stderr, "Expires list length %d\n", g_list_length(state->expires));
  return SMACQ_PASS;
}

static int active_init(struct smacq_init * context) {
  int argc = 0;
  char ** argv;
  struct state * state = context->state = g_new0(struct state, 1);
  state->env = context->env;

  {
	smacq_opt interval;

  	struct smacq_optval optvals[] = {
    		{ "t", &interval}, 
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

	state->interval = interval.timeval_t;
	state->hasinterval = (state->interval.tv_usec || state->interval.tv_sec); 

	state->timeseries = smacq_requirefield(state->env, "timeseries");
	state->activefield = smacq_requirefield(state->env, "active");
	state->activetype = smacq_requiretype(state->env, "int");
  }

  // Consume rest of arguments as field names
  fields_init(state->env, &state->fieldset, argc, argv);

  state->stats = bytes_hash_table_new(KEYBYTES, CHAIN, FREE);

  return 0;
}

static int active_shutdown(struct state * state) {
  return SMACQ_END;
}


static smacq_result active_produce(struct state * state, const dts_object ** datum, int * outchan) {
  return SMACQ_END;
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_active_table = {
  &active_produce, 
  &active_consume,
  &active_init,
  &active_shutdown
};

