#include <string>
#include <iostream>
#include <SmacqPyModule.h> 
#include <boost/python/object.hpp>
//#include<boost/python.hpp>

using namespace boost::python;

std::vector<SmacqPyModule *> avail_PyModules;

SmacqPyModule::SmacqPyModule(std::string name, object pymod_inst, struct SmacqModule::smacq_init * context) :
    SmacqModule(context), pymod(pymod_inst), name(name) {
      
    avail_PyModules.insert(avail_PyModules.begin(), this);
}

smacq_result SmacqPyModule::consume(DtsObject datum, int & outchan) {
    std::cout << "Inside consume, for some reason";
    return SMACQ_NONE;
}
