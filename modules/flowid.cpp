/*
 * This module assigns a flow-id number to objects based on a tuple of
 * fields.  Optionally, flows can be made to timeout after an idle
 * time.  In this case, an end-of-flow record will be output.
 *
*/

#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <smacq.h>
#include <FieldVec.h>
#include <FieldVec.h>
#include <dllist.h>
#include <produceq.h>

#define KEYBYTES 0
#define SMACQ_MODULE_IS_ANNOTATION 1

static struct smacq_options options[] = {
  {"t", {double_t:0}, "Threshold quiet time", SMACQ_OPT_TYPE_TIMEVAL},
  {"r", {boolean_t:0}, "Reverse each pair of fields for bidirectional flows", SMACQ_OPT_TYPE_BOOLEAN},
  END_SMACQ_OPTIONS
};

class SrcStat {
 public:
  struct timeval starttime, lasttime, timer_index;
  int id;

  unsigned int byte_count, byte_count_back;
  unsigned int packet_count, packet_count_back;
  FieldVec fields;

  struct element * hash_entry;
  struct list_element * timer_entry;

};

SMACQ_MODULE(flowid,
  PROTO_CTOR(flowid);
  PROTO_DTOR(flowid);
  PROTO_CONSUME();
  PROTO_PRODUCE();

  FieldVec fieldvec;
  FieldVec fieldvec2;

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
  dts_typeid id_type;
  dts_field flowid_field;
  dts_typeid refresh_type;
  dts_field start_field;
  dts_field finish_field;
  dts_typeid timeval_type;
  dts_field ts_field; // Field number
  dts_field len_field;
  dts_typeid len_type;
  dts_field byte_count_field;
  dts_field byte_count_back_field;
  dts_field packet_count_field;
  dts_field packet_count_back_field;

  struct list timers;

  FieldVecHash<SrcStat*> stats;
  
  int reverse;

  void attach_stats(SrcStat * s, DtsObject datum);
  void output(SrcStat * s);
  void finalize(SrcStat * s);
  SrcStat * stats_lookup(DtsObject datum, int * swapped);
  void timers_manage();
); 

static void timeval_minus(struct timeval x, struct timeval y, struct timeval * result) {
  *result = x;
  result->tv_sec -= y.tv_sec; // No spec for underflow

  if (y.tv_usec > result->tv_usec) {
    result->tv_sec--;
    result->tv_usec += (int)1e6;
  }
  result->tv_usec -= y.tv_usec;
  
  return;
}

static int timeval_past(struct timeval x, struct timeval y) {
  if (x.tv_sec > y.tv_sec) return 1;
  if ((x.tv_sec == y.tv_sec) && (x.tv_usec > y.tv_usec)) return 1;
  return 0;
}

void flowidModule::attach_stats(SrcStat * s, DtsObject datum) {
  DtsObject msgdata;

  msgdata = dts->construct(timeval_type, &s->starttime);
  datum->attach_field(start_field, msgdata);
  
  msgdata = dts->construct(len_type, &s->byte_count);
  datum->attach_field(byte_count_field, msgdata);
  
  msgdata = dts->construct(len_type, &s->byte_count_back);
  datum->attach_field(byte_count_back_field, msgdata);
  
  msgdata = dts->construct(len_type, &s->packet_count);
  datum->attach_field(packet_count_field, msgdata);
  
  msgdata = dts->construct(len_type, &s->packet_count_back);
  datum->attach_field(packet_count_back_field, msgdata);

}

inline void flowidModule::output(SrcStat * s) {
      // Output refresh record
      DtsObject msgdata;
      DtsObject refresh = dts->construct(refresh_type, NULL);
      DtsObjectVec ov = s->fields.getobjs();

      msgdata = dts->construct(id_type, &s->id);
      refresh->attach_field(flowid_field, msgdata);

      DtsObjectVec::iterator i;
      int j = 0;
      for (i = ov.begin(); 
	   i != ov.end(); 
	   i++) {
	refresh->attach_field(s->fields[j]->num, *i);
	j++;
      }

      msgdata = dts->construct(timeval_type, &s->lasttime);
      refresh->attach_field(finish_field, msgdata);

      //refresh->attach_field(ts_field, msgdata);
      
  
      attach_stats(s, refresh);
      //fprintf(stderr, "enqueuing %p\n", refresh);
      smacq_produce_enqueue(&outputq, refresh, -1);
}

inline void flowidModule::finalize(SrcStat * s) {
    output(s);

    active--;
    /* fprintf(stderr, "%d active flows, timer list has %d\n", active, list_length(&timers));*/

    // Cleanup
    /* Don't have to decref fields, since their refcount will be picked up from being attached in the output routine */

    delete s;
}

inline SrcStat * flowidModule::stats_lookup(DtsObject datum, int * swapped) {
  FieldVecHash<SrcStat *>::iterator s = stats.find(fieldvec);
  if (s != stats.end()) return s->second;
  
  /* Try reverse */
  if (reverse) {
    fieldvec2.getfields(datum);
    s = stats.find(fieldvec2);
    if (s != stats.end()) {
      *swapped = 1;
      return s->second;
    }
  }
  
  return NULL;
}


void flowidModule::timers_manage() {
    //fprintf(stderr, "managing of list\n");
    while (1) {
        SrcStat * s = (SrcStat*)list_peek_value(&timers);
	if (!s) {
		//fprintf(stderr, "end of list\n");
		break;
	}

	if (timeval_past(s->timer_index, edge)) {
		//fprintf(stderr, "stopping at %p time %d now %d \n", s, s->lasttime.tv_sec, edge.tv_sec);
		break;
	}

	if (timeval_past(s->lasttime, edge)) {
		/* Need to reset timer for this guy */
		s->timer_index = s->lasttime;
		list_append_element(&timers, list_pop_element(&timers));

		break;
	}

	/* If we got here, then the flow has timed-out */

	/* Remove from all datastructures */
	list_element_free(&timers, list_pop_element(&timers));

	/* Output and free */
	finalize(s);

	/* fprintf(stderr, "remove element %p hash %p\n", s, s->hash_entry); */
	stats.erase(s->fields);
    }
}

/*
 * Plan of attack: 
 *
 * 1) Find out what time it is now.
 * 2) See if the current object belongs to an active flow, else create one
 * 3) Timout idle flows and send out a final record
 *
 */
smacq_result flowidModule::consume(DtsObject datum, int & outchan) {
  SrcStat * s;
  smacq_result status = SMACQ_FREE;

  DtsObject field;
  DtsObject msgdata;
  struct timeval tsnow;
  int size;
  int swapped = 0;

  // Get current time
  if (! (field = datum->getfield(ts_field))) {
    fprintf(stderr, "error: timeseries not available\n");
    return SMACQ_PASS;
  } else {
    tsnow = dts_data_as(field, struct timeval);
    
  }

  // Get current size
  if (! (field = datum->getfield(len_field))) {
    fprintf(stderr, "error: len not available\n");
    return SMACQ_PASS;
  } else {
    size = dts_data_as(field, unsigned int);
    
  }

  // Update clock
  if (hasinterval) 
    timeval_minus(tsnow, interval, &edge);

  fieldvec.getfields(datum);
  if (fieldvec.has_undefined()) {
    /* Not all fields present */
    //fprintf(stderr, "Skipping datum\n");

	return SMACQ_FREE;
  }
  s = stats_lookup(datum, &swapped);

  // Make new entry if necessary 
  if (!s) {
      s = new SrcStat;
      //fprintf(stderr, "new %p\n", s);
      
      s->id = flowid++;
      s->starttime = tsnow;
      s->lasttime = tsnow;
      s->fields = fieldvec;
      
      stats[fieldvec] = s;

      if (hasinterval) {
	s->timer_entry = list_append_value(&timers, s);
	s->timer_index = s->lasttime;
      }

      active++;
      status = SMACQ_PASS;
  } else {
      // Update state
      s->lasttime = tsnow;

      /* Cheaper not to move to end of timer list.
      if (hasinterval) {
      	list_remove_element(&timers, s->timer_entry);
	list_append_element(&timers, s->timer_entry);
      }
      */

      //fprintf(stderr, "update %p\n", s);
  }


  if (swapped) {
    s->byte_count_back += size;
    s->packet_count_back++;
  } else {
    s->byte_count += size;
    s->packet_count++;
  }

  if (status & SMACQ_PASS) {
    // Attach flowid to this datum
    msgdata = dts->construct(id_type, &s->id);
    datum->attach_field(flowid_field, msgdata);
    attach_stats(s, datum);
  }

  if (hasinterval) 
      timers_manage();

  status |= smacq_produce_canproduce(&outputq);
  /* fprintf(stderr, "consume PRODUCE? %d\n", status & (SMACQ_PRODUCE|SMACQ_CANPRODUCE)); */

  //fprintf(stderr, "Expires list length %d\n", list_length(expires));
  return status;
}

flowidModule::flowidModule(struct smacq_init * context) : SmacqModule(context) {
  int argc = 0;
  char ** argv;

  refresh_type = dts->requiretype("refresh");

  flowid_field = dts->requirefield("flowid");
  id_type = dts->requiretype("int");

  len_type = dts->requiretype("uint32");

  byte_count_field = dts->requirefield("bytes");
  byte_count_back_field = dts->requirefield("bytesback");

  packet_count_field = dts->requirefield("packets");
  packet_count_back_field = dts->requirefield("packetsback");

  ts_field = dts->requirefield("timeseries");
  timeval_type = dts->requiretype("timeval");

  start_field = dts->requirefield("start");
  finish_field = dts->requirefield("finish");
  len_field = dts->requirefield("len");

  {
	smacq_opt intervalo, reverseo;

  	struct smacq_optval optvals[] = {
    		{ "t", &intervalo}, 
    		{ "r", &reverseo}, 
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

	interval = intervalo.timeval_t;
	hasinterval = (interval.tv_usec || interval.tv_sec); 
	reverse = reverseo.boolean_t;
  }

  // Consume rest of arguments as field names
  fieldvec.init(dts, argc, argv);

  if (reverse) {
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
    fieldvec2.init(dts, argc, rargv);
    free(rargv);
  }

  list_init(&timers);
}

flowidModule::~flowidModule() {
  list_free(&timers);
}

smacq_result flowidModule::produce(DtsObject & datum, int & outchan) {
  if (smacq_produce_canproduce(&outputq)) {
    return smacq_produce_dequeue(&outputq, datum, outchan);
  } else {
    /* fprintf(stderr, "flowid: produce called with nothing in queue.  Outputing everything in current table!\n"); */

    FieldVecHash<SrcStat*>::iterator i;
    for (i=stats.begin(); i!=stats.end(); i++) {
      finalize(i->second);
    }

    list_free(&timers);

    if (!smacq_produce_canproduce(&outputq))
	    return (smacq_result)(SMACQ_FREE|SMACQ_END);

    return produce(datum, outchan);
  }
}

