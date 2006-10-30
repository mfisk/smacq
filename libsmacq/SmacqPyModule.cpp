#include <string>
#include <iostream>
#include <SmacqPyModule.h> 
//#include <boost/python/object.hpp>
//#include<boost/python.hpp>

using namespace boost::python;

/// Holds the list of python smacq modules
std::vector<SmacqPyModule *> avail_PyModules;

SmacqPyModule::SmacqPyModule(std::string name, object pymod_inst) :
    pymod(pymod_inst), name(name) {
      
    avail_PyModules.insert(avail_PyModules.begin(), this);
}

/// Search through the list of available python modules.  If the module exists, return the
/// generic python module (the specific pymodule will be set later 
void * smacq_find_pymodule(lt_dlhandle* gmodulep, char * envvar, char * envdefault, 
                           char * modformat, char * symformat, char * sym) {
    int i, num_pymods = avail_PyModules.size(); 
   
    for (i = 0; i < num_pymods; i++) {
        std::cout << "Looking at pymod: " << avail_PyModules.at(i)->name.c_str() << ", and comparing it to: " << sym << "!!!" << std::endl;
        if (strcmp(avail_PyModules.at(i)->name.c_str() , sym) == 0 ) {
            std::cout << "Found it\n";
            return smacq_find_module(gmodulep, envvar, envdefault, modformat, symformat, "pymod");
        }
    }
    
    return NULL;

}

