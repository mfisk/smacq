#include <stdlib.h>
#include <assert.h>
#include <smacq.h>
#include <FieldVec.h>

struct obj_list{
  DtsObject obj;
  struct obj_list * next;
};

SMACQ_MODULE(pdf,
  PROTO_CTOR(pdf);
  PROTO_CONSUME();
  PROTO_PRODUCE();

  FieldVec fieldvec;
  
  struct obj_list * outputq;
  struct obj_list * list;

  dts_typeid refreshtype;
  dts_typeid probtype;
  DtsField probfield;
  DtsField countfield;
  unsigned long long total;

  void compute_all();
); 

void pdfModule::compute_all() {
  DtsObject count; 
  DtsObject pfield; 
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
	
  }

  total = 0;
}
  
smacq_result pdfModule::consume(DtsObject datum, int & outchan) {
  DtsObject count; 
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
  	struct obj_list * newo = new obj_list;
  	newo->obj = datum;
  	newo->next = list;
  	list = newo;

	

  	total += dts_data_as(count, int);
	
  }

  if (outputq) 
	res |= SMACQ_PRODUCE;

  return(res);
}

pdfModule::pdfModule(struct SmacqModule::smacq_init * context) 
      : SmacqModule(context), 
  	refreshtype(dts->requiretype("refresh")),
  	probtype(dts->requiretype("double")),
  	probfield(dts->requirefield("probability")),
	countfield(dts->requirefield("count"))
{}

smacq_result pdfModule::produce(DtsObject & datum, int & outchan) {
  if (!outputq) {
    compute_all();
  }
    
  if (outputq) {
    datum = outputq->obj;
    outputq = outputq->next;
  } else {
    return SMACQ_END;
  }


  return (smacq_result)(SMACQ_PASS|(outputq ? SMACQ_PRODUCE : 0));
}

