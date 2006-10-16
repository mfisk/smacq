#include <SmacqModule-interface.h>
#include <smacq_result.h>
#include <DtsField.h>
#include <SmacqScheduler.h>
#include <dts.h>
#include <string>
#include <vector>
#include <boost/python/object.hpp>

using namespace boost::python;

/// This class is used to store the basic information about a smacq module written in Python.
/// To call and define python smacq modules, you must be using pysmacq, the python smacq library.
/// They do not instantiate the python interpreter, but instead rely on being called by a python
/// interpreter that has already been instantiated.  I think.  (PF)
class SmacqPyModule : public SmacqModule {
  public:

    SmacqPyModule(std::string name, object pymod_inst, smacq_init_type * context);

    void test(std::string data) {
        pymod(data);
    }

    smacq_result consume(DtsObject datum, int & outchan);

  private:
  
    // This holds an instance of a python smacq module class.  The module class should inherit from
    // the python SmacqModule class.
    object pymod;

    // This string is used both to look up this module, and to call it.  Wow.  Reasonable name checking 
    // occurs in python where it's easier to do.
    std::string name;

};

//std::vector<SmacqPyModule*> avail_PyModules;
