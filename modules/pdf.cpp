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
#include "bytehash.h"

/* Programming constants */

#define VECTORSIZE 21
#define ALARM_BITS 15
#define KEYBYTES 128

static struct smacq_options options[] = {
  {NULL, {string_t:NULL}, NULL, 0}
};

struct obj_list{
  DtsObject * obj;
  struct obj_list * next;
};

SMACQ_MODULE(pdf,
  PROTO_CTOR(pdf);
  PROTO_CONSUME();
  PROTO_PRODUCE();

  struct fieldset fieldset;
  struct iovec_hash *last;
  
  struct obj_list * outputq;
  struct obj_list * list;

  dts_typeid refreshtype;
  dts_typeid probtype;
  dts_field probfield;
  dts_field countfield;
  unsigned long long total;

  void compute_all();
); 

void pdfModule::compute_all() {
  DtsObject * count; 
  DtsObject * pfield; 
  struct obj_list * n;
  double p;

  outputq = list;
  list = NULL;

  for (n = outputq; n; n = n->next) {
  	if (! (count = n->obj->getfield(countfield))) {
		assert(0);
	}
	p = (double)(dts_data_as(count, int)) / (double)(total);
	//fprintf(stderr, "%d / %lld = %g\n", *(int*)(count.data), total, p);
	pfield = dts->construct(probtype, &p);
	n->obj->attach_field(probfield, pfield); 
	count->decref();
  }

  total = 0;
}
  
smacq_result pdfModule::consume(DtsObject * datum, int * outchan) {
  DtsObject * count; 
  smacq_result res = SMACQ_FREE;

  if (! (count = datum->getfield(countfield))) {
      if (datum->gettype() == refreshtype) {
          compute_all();
	  res = SMACQ_PASS;
      } else {
      	fprintf(stderr, "error: timeseries not available\n");
      	return SMACQ_PASS;
      }
  } else {
  	struct obj_list * newo = g_new(struct obj_list, 1);
  	newo->obj = datum;
  	newo->next = list;
  	list = newo;

	datum->incref();

  	total += dts_data_as(count, int);
	count->decref();
  }

  if (outputq) 
	res |= SMACQ_PRODUCE;

  return(res);
}

pdfModule::pdfModule(struct smacq_init * context) 
      : SmacqModule(context), 
	countfield(dts->requirefield("count")),
  	probfield(dts->requirefield("probability")),
  	probtype(dts->requiretype("double")),
  	refreshtype(dts->requiretype("refresh"))
{}

smacq_result pdfModule::produce(DtsObject ** datum, int * outchan) {
  if (!outputq) {
    compute_all();
  }
    
  if (outputq) {
    *datum = outputq->obj;
    outputq = outputq->next;
  } else {
    return SMACQ_END;
  }


  return(SMACQ_PASS|(outputq ? SMACQ_PRODUCE : 0));
}

