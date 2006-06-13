#ifndef SMACQ_MODULE_INTERFACE_H
#define SMACQ_MODULE_INTERFACE_H

#include <smacq.h>
#include <smacq_result.h>
#include <smacq_args.h>
#include <DtsObject.h>
#include <DynamicArray.h>

#include <list>
#include <utility>

///
/// A virtual base class for SMACQ modules.
///

/*!
  This document describes the programming interface used by authors of 
  dataflow modules.  These modules are dynamically loaded and may be
  instantiated multiple times.  Global and static variables are therefore
  deprecated for most cases.
*/

class SmacqModule {
 public:

  /// This context structure is passed to SmacqModule constructors.
  /// It will be destroyed after the constructor returns, but the
  /// elements it points to are guaranteed to be available during the
  /// lifetime of the object.
  struct smacq_init {
    SmacqScheduler * scheduler;
    bool isfirst;
    bool islast;
    char ** argv;
    int argc;
    DTS * dts;
    SmacqGraph * self;
  };

  /// SMACQ modules are object files that can be statically or
  /// dynamically loaded into a program.  Each module should use the
  /// SMACQ_MODULE() macro to make sure that the module defines a
  /// constructor function that instantiates the class.
  typedef SmacqModule*  constructor_fn(struct smacq_init *);
  
  /// The algebra element is optional and is used only by the dataflow
  /// optimizer.  The following elements of the algebra structure are
  /// as follows: Vector specifies that the module can be used with a
  /// single input and a single output, or can be used with a vector
  /// of sets of arguments separated by semicolons and a corresponding
  /// vector of output channels.  Boolean specifies that the module
  /// merely filters out some data and can be reordered in the
  /// dataflow by an optimizer.  Demux specifies that the module
  /// demultiplexes output data among multiple output chanels.  If a
  /// demux module fails to set the demux bit, then the optimizer may
  /// produce disfunctional output.
  struct algebra {
    unsigned int stateless:1;
    unsigned int vector:1;
    unsigned int annotation:1;
    unsigned int demux:1;
  };

  /// Most subclasses will define their own constructor which will
  /// initialize the instance based on the given context.
  SmacqModule(struct smacq_init * context);
  
  /// A subclass can have its own destructor
  virtual ~SmacqModule();

  /// The consume() method is called when there is new data for a
  /// module to process.  It is passed a pointer to a data object and
  /// a reference to an output channel descriptor.  The return code
  /// should be SMACQ_PASS if the object is not filtered out and
  /// SMACQ_FREE if it is.  In addition, the return code can be OR'd
  /// with the following flags: SMACQ_ERROR specifies that there was a
  /// fatel error in the module.  SMACQ_END signifies that the module
  /// wishes to never be called again.  
  virtual smacq_result consume(DtsObject datum, int & outchan);
  
  /// The produce() method is called when SMACQ expects an object to
  /// produce new data. 
  virtual smacq_result produce(DtsObject & datump, int & outchan);

  /// This method is called by the join optimizer.
  virtual bool usesOtherFields(DtsField f);

 protected:
  class UsesArray : public DynamicArray<bool> {
  public: 
    bool otherEntry(unsigned int f) const;
  };

  UsesArray usesFields;

  void comp_uses(dts_comparison * c);

  /// Return a newly constructed dts_comparison datastructure from the given arguments
  dts_comparison * SmacqModule::parse_tests(int argc, char ** argv);

  /// This method wraps DTS::usesfield() but keeps track of what 
  /// this module uses.
  virtual DtsField usesfield(char * name);

  /// Each module instance runs in the context of a DTS instance.
  DTS * dts;

  /// Each module instance is run by a scheduler.
  SmacqScheduler * scheduler;

  /// A pointer to ourself in the current dataflow graph
  SmacqGraph * self;
  
  /// Enqueue an object for output to the specified output channel.
  void enqueue(DtsObject &, int outchan = 0);
};

struct smacq_functions {
  SmacqModule::constructor_fn * constructor;
  struct SmacqModule::algebra algebra;
  /* Put constructor and algebra first so that we can use partial initializers in g++ */
};

#endif
