#ifndef SMACQPYMODULE
#define SMACQPYMODULE
#include <SmacqModule-interface.h>
#include <smacq_result.h>
#include <DtsField.h>
#include <SmacqScheduler.h>
#include <dts.h>
#include <string>
#include <vector>
#include <boost/python.hpp>

//using namespace boost::python;

/// This class is used to store the basic information about a smacq module written in Python.
/// To call and define python smacq modules, you must be using pysmacq, the python smacq library.
/// They do not instantiate the python interpreter, but instead rely on being called by a python
/// interpreter that has already been instantiated.  I think.  (PF)
class SmacqPyModule {
  public:

    SmacqPyModule(std::string name, boost::python::object pymod_inst);

    void test(std::string data) {
        pymod.attr("bleh")(data);
    }

    // This holds an instance of a python smacq module class.  The module class should inherit from
    // the python SmacqModule class.
    boost::python::object pymod;

    // This string is used both to look up this module, and to call it.  Wow.  Reasonable name checking 
    // occurs in python where it's easier to do.
    std::string name;

};

void * smacq_find_pymodule(lt_dlhandle* gmodulep, char * envvar, char * envdefault,
                         char * modformat, char * symformat, char * sym);

#endif 
