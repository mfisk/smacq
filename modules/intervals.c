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
  {"u", {double_t:0}, "Update period", SMACQ_OPT_TYPE_TIMEVAL},
  {NULL, {string_t:NULL}, NULL, 0}
};

struct srcstat {
  struct timeval starttime, lasttime;
  int count;
  int inlist;
};

struct state {
  smacq_environment * env;
  struct fieldset fieldset;
  GHashTableofBytes *stats;

  int hasupdateinterval;
  struct timeval updateinterval, nextupdate;
  int istarted;
  FILE * printfd;

  struct srcstat unified;

  struct timeval interval;
  int hasinterval;

  GList * expires;

  dts_field timeseries; // Field number
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

static void print_extent(gpointer key, gpointer value, gpointer userdata) {
  struct state * state = userdata;
  struct srcstat * stats = value;
  int i;

  if (key == NULL) { // We're talking about the current value, not a table lookup
      const dts_object * d;
      i = 0;
      while((i = smacq_nextfielddata(&state->fieldset, &d, i))) {
	d = smacq_getfield(state->env, d, smacq_requirefield(state->env, "string"), NULL);
	fprintf(state->printfd, "%s ", (char*)d->data);
      }
  } else {
    int offset = 0;
    struct bytedata * b = key;
    const dts_object * lastd;
    i=0;

    while((i = smacq_nextfielddata(&state->fieldset, &lastd, i))) {
        const dts_object * d = (dts_object*)lastd;
	const dts_object * dstr = NULL;

        assert(offset < b->len);
        ((dts_object*)d)->data = b->bytes+offset;
        offset += d->len;

        dstr = smacq_getfield(state->env, d, smacq_requirefield(state->env, "string"), NULL);
        fprintf(state->printfd, "%s ", (char*)dstr->data);
	dts_decref(dstr);
    }

  }
  fprintf(state->printfd, "\t%lu -\t%lu\t%u\n", stats->starttime.tv_sec, stats->lasttime.tv_sec, stats->count);
}



static void timeval_inc(struct timeval * x, struct timeval y) {
  x->tv_usec += y.tv_usec;
  x->tv_sec += y.tv_sec;

  if (x->tv_usec > 1000000) {
    x->tv_sec++;
    x->tv_usec -= 1000000;
  }

}

static int timeval_ge(struct timeval x, struct timeval y) {
  if (x.tv_sec > y.tv_sec) return 1;
  if ((x.tv_sec == y.tv_sec) && (x.tv_usec >= y.tv_usec)) return 1;
  return 0;
}
  
static int timeval_past(struct timeval x, struct timeval y) {
  if (x.tv_sec > y.tv_sec) return 1;
  if ((x.tv_sec == y.tv_sec) && (x.tv_usec > y.tv_usec)) return 1;
  return 0;
}

static void printout(struct state * state, int update) {
  if (update) {
    state->printfd = fopen("/var/tmp/current.tmp", "w");
    assert(state->printfd >= 0);
  } else {
    state->printfd = stdout;
  }

  if (!state->fieldset.num) { 
    print_extent(NULL, &state->unified, state);
  } else {
    //if (state->hasupdateinterval) 
      //printf("\fExtents at %lu.%06lu\n", state->nextupdate.tv_sec, state->nextupdate.tv_usec);

    bytes_hash_table_foreach(state->stats, print_extent, state);
  }

  if (update) {
	fclose(state->printfd);

  	// Not: Update current even if !update (clear current on final output)
  	rename("/var/tmp/current.tmp", "/var/tmp/current");
  }
}
  
static smacq_result intervals_consume(struct state * state, const dts_object * datum, int * outchan) {
  struct iovec * domainv = NULL;
  struct srcstat * s;

  struct timeval endtime;
  dts_object field;
  struct timeval * value;

  if (!smacq_getfield(state->env, datum, state->timeseries, &field)) {
    fprintf(stderr, "error: timeseries not available\n");
  } else {
    value = (struct timeval*)field.data;
    assert(field.len == sizeof(struct timeval));
    
    if (state->hasupdateinterval) {
      if (!state->istarted) {
	state->istarted = 1;
	state->nextupdate = *value;
	timeval_inc(&state->nextupdate, state->updateinterval);
      } else if (timeval_ge(*value, state->nextupdate)) {
	// Print counters
	printout(state, 1);
	
	timeval_inc(&state->nextupdate, state->updateinterval);
	while (timeval_past(*value, state->nextupdate)) { // gap in timeseries
	  timeval_inc(&state->nextupdate, state->updateinterval);
	}
      }
    }
  }

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

  if (!s) {
    s = g_new(struct srcstat,1);
    bytes_hash_table_insertv(state->stats, domainv, state->fieldset.num, s);

    s->starttime = *value;
    s->count = 1;
  } else {
    if (state->hasinterval) {
      endtime = s->lasttime;
      timeval_inc(&endtime, state->interval);
    
      if (!timeval_past(*value, endtime)) {
	//fprintf(stdout, "last was %lu, now %lu\n", s->lasttime.tv_sec, value->tv_sec);
	print_extent(NULL, s, state);
        s->count++;
      } else {
      	// Reset extent
      	s->starttime = *value;
      	s->count = 1; 
      }
    } else {
      s->count++;
    }
  }
  s->lasttime = *value;

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
	
	if (!this || timeval_past(this->lasttime, thold) ) break;
	print_extent(NULL, this, state);
	state->expires = g_list_delete_link(state->expires, thisel);
      }
    }
  }
  //fprintf(stderr, "Expires list length %d\n", g_list_length(state->expires));
  return SMACQ_FREE;
}

static int intervals_init(struct smacq_init * context) {
  int argc = 0;
  char ** argv;
  struct state * state = context->state = g_new0(struct state, 1);
  state->env = context->env;

  {
	smacq_opt interval, updateinterval;

  	struct smacq_optval optvals[] = {
    		{ "t", &interval}, 
		{ "u", &updateinterval },
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

	state->interval = interval.timeval_t;
	state->hasinterval = (state->interval.tv_usec || state->interval.tv_sec); 

	state->updateinterval = updateinterval.timeval_t;
	if ((updateinterval.timeval_t.tv_sec != 0) || (updateinterval.timeval_t.tv_usec != 0)) {
	  state->hasupdateinterval = 1;
	} else {
	  state->hasupdateinterval = 0;
	}

	state->timeseries = smacq_requirefield(state->env, "timeseries");
  }

  // Consume rest of arguments as fieldnames
  fields_init(state->env, &state->fieldset, argc, argv);

  state->stats = bytes_hash_table_new(KEYBYTES, CHAIN, FREE);
  state->printfd = stdout;

  return 0;
}

static int intervals_shutdown(struct state * state) {
  // Print counters
  fprintf(stderr, "SHUTDOWN\n");
  printout(state, 0);

  return 0;
}


static smacq_result intervals_produce(struct state * state, const dts_object ** datum, int * outchan) {
  return SMACQ_END;
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_intervals_table = {
  &intervals_produce, 
  &intervals_consume,
  &intervals_init,
  &intervals_shutdown
};

