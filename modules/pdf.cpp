#include <stdlib.h>
#include <assert.h>
#include <SmacqModule.h>
#include <FieldVec.h>

SMACQ_MODULE(pdf,
  PROTO_CTOR(pdf);
  PROTO_DTOR(pdf);
  PROTO_CONSUME();

  FieldVec fieldvec;
  
  std::vector<DtsObject> list;

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
  std::vector<DtsObject>::iterator n;
  double p;

  for (n = list.begin(); n != list.end(); ++n) {
  	if (! (count = (*n)->getfield(countfield))) {
		assert(0);
	}
	p = (double)(dts_data_as(count, int)) / (double)(total);
	//fprintf(stderr, "%d / %lld = %g\n", *(int*)(count.data), total, p);
	pfield = dts->construct(probtype, &p);
	(*n)->attach_field(probfield, pfield); 

	enqueue(*n);	
  }
  
  list.resize(0);
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
	list.push_back(datum);
  	total += dts_data_as(count, int);
  }

  return(res);
}

pdfModule::pdfModule(struct SmacqModule::smacq_init * context) 
      : SmacqModule(context), 
  	refreshtype(dts->requiretype("refresh")),
  	probtype(dts->requiretype("double")),
  	probfield(dts->requirefield("probability")),
	countfield(dts->requirefield("count"))
{}

pdfModule::~pdfModule() {
    compute_all();
}

