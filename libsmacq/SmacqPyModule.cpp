#include <string>
#include <iostream>
#include <SmacqPyModule.h> 
//#include <boost/python/object.hpp>
//#include<boost/python.hpp>

using namespace boost::python;

std::vector<SmacqPyModule *> avail_PyModules;

SmacqPyModule::SmacqPyModule(std::string name, object pymod_inst, smacq_init * context) :
    pymod(pymod_inst), name(name) {
    int a;
    a = 1;
      
    avail_PyModules.insert(avail_PyModules.begin(), this);
}

smacq_result SmacqPyModule::consume(DtsObject datum, int & outchan) {
    std::cout << "Inside consume, for some reason";
    return SMACQ_NONE;
}

/// Search through the list of available python modules.  If the module exists, return the
/// generic python module (the specific pymodule will be set later 
void * smacq_find_pymodule(lt_dlhandle* gmodulep, char * envvar, char * envdefault, 
                           char * modformat, char * symformat, char * sym) {
    int i, num_pymods = avail_PyModules.size(); 
   
    for (i = 0; i < num_pymods; i++) {
        if (strcmp(avail_PyModules.at(i)->name.c_str() , sym) ) {
            return smacq_find_module(gmodulep, envvar, envdefault, modformat, symformat, sym);
        }
    }
    
    return NULL;

}
