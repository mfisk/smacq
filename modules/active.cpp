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
#include <FieldVec.h>
#include <FieldVec.h>

/* Programming constants */

#define VECTORSIZE 21
#define ALARM_BITS 15
#define KEYBYTES 128

static struct smacq_options options[] = {
  {"t", {double_t:0}, "Threshold quiet time", SMACQ_OPT_TYPE_TIMEVAL},
  END_SMACQ_OPTIONS
};

struct srcstat {
  struct timeval starttime, lasttime;
  int count;
  int inlist;
  int expired;
};

struct state {
  DTS * env;
  FieldVec fieldvec;
  FieldVecHash *stats;

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
smacq_result activeModule::consume(DtsObject datum, int & outchan) {
  struct srcstat * s;

  DtsObject field;
  DtsObject activefielddata;
  struct timeval * value;

  // Get current time
  if (!(field = datum->getfield(timeseries, &NULL))) {
    fprintf(stderr, "error: timeseries not available\n");
    return SMACQ_PASS;
  } else {
    value = (struct timeval*)field->getdata();
    assert(field.len == sizeof(struct timeval));
  }

  // Find this entry
  if (!fieldvec.num) {
    s = &(unified);
  } else {
    fieldvec.getfields(datum);

    s = stats->lookup(fieldvec);
  }

  // Manage expires list
  if (hasinterval) {
    expires = g_list_remove(expires, s);
    expires = g_list_append(expires, s);
    {
      struct timeval thold;
      GList * thisel;
      timeval_minus(*value, interval, &thold);
      
      while((thisel = g_list_first(expires))) {
	struct srcstat * this = thisel->data;

	if (!this || timeval_past(this->lasttime, thold)) break; // Future stuff to expire
	active--;
	this->expired = 1;
	expires = g_list_delete_link(expires, thisel);
      }
    }
  }

  // Garbage collect
  if (! --gc_count) {
  	stats->foreach_remove(isexpired, NULL);
   	gc_count = 1000;
  }

  if (!s) {
    s = g_new(struct srcstat,1);
    stats[domainv] = s;
    active++;
  } else if (s->expired) {
    s->expired = 0;
    active++;
  }
  s->lasttime = *value;

  // Attach active count to this datum
  activefielddata = dts->construct(activetype, &active);
  datum->attach_field(activefield, activefielddata);

  //fprintf(stderr, "Expires list length %d\n", g_list_length(expires));
  return SMACQ_PASS;
}

activeModule::activeModule(struct smacq_init * context) {
  int argc = 0;
  char ** argv;

  {
	smacq_opt interval;

  	struct smacq_optval optvals[] = {
    		{ "t", &interval}, 
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

	interval = interval.timeval_t;
	hasinterval = (interval.tv_usec || interval.tv_sec); 

	timeseries = dts->requirefield("timeseries");
	activefield = dts->requirefield("active");
	activetype = dts->requiretype("int");
  }

  // Consume rest of arguments as field names
  fieldvec.init(dts, argc, argv);

  stats = new BytesHashTable(KEYBYTES, CHAIN|FREE);

  return 0;
}

activeModule::~activeModule(struct state * state) {
  return SMACQ_END;
}


smacq_result activeModule::produce(DtsObject & datum, int & outchan) {
  return SMACQ_END;
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_active_table = {
  &active_produce, 
  &active_consume,
  &active_init,
  &active_shutdown
};

