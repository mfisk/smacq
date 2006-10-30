#include <stdio.h>
#include <boost/python/object.hpp>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <vector>
#include <SmacqPyModule.h>
#include <pysmacq.h>
#include <SmacqModule.h>

using namespace boost::python;
extern std::vector<SmacqPyModule *> avail_PyModules;

SMACQ_MODULE(pymod,
  PROTO_CTOR(pymod);
  PROTO_DTOR(pymod);
  PROTO_CONSUME();
  PROTO_PRODUCE();
  object pymod_obj;

);

smacq_result pymodModule::consume(DtsObject datum, int & outchan) {
  smacq_result temp;
  printf("In pymod.cpp: Calling Python consume method\n");
  temp = extract<smacq_result>(pymod_obj.attr("consume")(datum, outchan));
  printf("In pymod.cpp: Returning smacq_result\n");
  return temp;
}

smacq_result pymodModule::produce(DtsObject & pdatum, int & outchan) {
  object py_result = pymod_obj.attr("produce")(outchan);

  pdatum = extract<DtsObject &>(py_result[0]);
  
  return extract<smacq_result>(py_result[1]);
}

pymodModule::pymodModule(smacq_init * context) : SmacqModule(context) {
/*  int argc = context->argc-1;
  char ** argv = context->argv+1; */
  std::string pymod_name = context->argv[0];
  bool found = false;

  int i, num_pymods = avail_PyModules.size();
  for (i = 0; i < num_pymods; i++) {
      printf("Looking at %s, comparing it to %s", avail_PyModules.at(i)->name.c_str(), pymod_name.c_str());
      if (strcmp(avail_PyModules.at(i)->name.c_str(), pymod_name.c_str()) == 0) {
          pymod_obj = avail_PyModules.at(i)->pymod(scheduler, self);
          found = true;
          break;
      }
  }
  
  assert(found);

}

pymodModule::~pymodModule() {
    printf("In pymod.cpp: Destroying a pymod\n");
}

/*
/// Provides an interface for using enqueue functionality in python.
void pymodModule::py_enqueue(DtsObject, outchan) {
    enqueue(DtsObject, outchan);
    return;
}
*/
