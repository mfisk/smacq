#ifndef SMACQMODULE_H
#define SMACQMODULE_H

#ifdef __cplusplus

class SmacqModule;

#include <smacq.h>
#include <list>
#include <utility>

/* Usage:

SMACQ_MODULE(foo,
  private:
    ...
);

*/

#define PROTO_CONSUME() public: smacq_result consume(DtsObject, int&); private:
#define PROTO_PRODUCE() public: smacq_result produce(DtsObject&, int&); private:
#define PROTO_CTOR(name) public: name##Module::name##Module(struct SmacqModule::smacq_init *); private:
#define PROTO_DTOR(name) public: name##Module::~name##Module(); private:

#define SMACQ_MODULE(name,defs) \
 class name##Module : public SmacqModule {	\
  private:					\
      defs					\
  };					        \
  EXPORT_SMACQ_MODULE(name, MODULE_ALGEBRA)

#define SMACQ_MODULE_THREAD(name,defs) \
 class name##Module : public ThreadedSmacqModule {	\
  public:					\
   smacq_result thread(struct SmacqModule::smacq_init* context);		\
    name##Module(struct SmacqModule::smacq_init * context) : ThreadedSmacqModule(context) {} \
  private:					\
      defs					\
  };					        \
  EXPORT_SMACQ_MODULE(name, MODULE_ALGEBRA)

#define MODULE_ALGEBRA { \
	stateless: SMACQ_MODULE_IS_STATELESS, \
	vector: SMACQ_MODULE_IS_VECTOR, \
 	annotation: SMACQ_MODULE_IS_ANNOTATION, \
	demux: SMACQ_MODULE_IS_DEMUX }

#define SMACQ_MODULE_IS_STATELESS 0
#define SMACQ_MODULE_IS_VECTOR 0
#define SMACQ_MODULE_IS_ANNOTATION 0
#define SMACQ_MODULE_IS_DEMUX 0

#define EXPORT_SMACQ_MODULE(name, alg)					\
  SMACQ_MODULE_CONSTRUCTOR(name);					\
  struct smacq_functions smacq_##name##_table = {			\
    constructor: &name##_constructor,					\
    algebra: alg							\
  };

#define SMACQ_MODULE_CONSTRUCTOR(name) \
  static SmacqModule * name##_constructor(struct SmacqModule::smacq_init * context) { \
    return new name##Module(context);					\
  }				

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

  struct ops {
    constructor_fn * constructor;
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
  /// wishes to never be called again.  SMACQ_PRODUCE signals that the
  /// object's produce() method should be called to return new data.
  /// SMACQ_CANPRODUCE signals that the produce() may be called to
  /// return new data.  
  virtual smacq_result consume(DtsObject datum, int & outchan);
  
  /// The produce() method is called when SMACQ expects an object to
  /// produce new data.  If a subclass does not implement a produce()
  /// method, the default implementation will return any objects that
  /// have been registered for output with enqueue().
  virtual smacq_result produce(DtsObject & datump, int & outchan);
  
 protected:
  /// Each module instance runs in the context of a DTS instance.
  DTS * dts;
  
  /// @name Output Queueing
  /// A module can internally queue output objects
  /// @{
  
  /// Return SMACQ_CANPRODUCE or SMACQ_NONE
  smacq_result canproduce();

  /// Dequeue an object for output.  Both the object reference output
  /// channel reference will be set if there is anything on the queue.
  /// SMACQ_PASS is returned on success and SMACQ_NONE is returned if
  /// the queue is empty.  If there is more than one item in the
  /// queue, then SMACQ_PRODUCE|SMACQ_PASS is returned.
  smacq_result dequeue(DtsObject &, int & outchan);

  /// Enqueue an object for output to the specified output channel.
  void enqueue(DtsObject &, int outchan);
  /// @}
  
 private:
  std::list<std::pair<DtsObject,int> > outputq;
};

inline SmacqModule::SmacqModule(struct smacq_init * context) {
	dts = context->dts;
}

inline SmacqModule::~SmacqModule() {}

inline smacq_result SmacqModule::consume(DtsObject datum, int & outchan) {
	return SMACQ_ERROR;
}

inline smacq_result SmacqModule::produce(DtsObject & datum, int & outchan) {
	if (outputq.empty()) {
		return SMACQ_END;
	} else {
		return dequeue(datum, outchan);
	}
}

inline smacq_result SmacqModule::canproduce() {
	if (outputq.empty()) {
		return SMACQ_NONE;
	} else {
		return SMACQ_PRODUCE;
	}
}
	
inline void SmacqModule::enqueue(DtsObject & datum, int outchan) {
	outputq.push_back(std::pair<DtsObject,int>(datum,outchan));
}

inline smacq_result SmacqModule::dequeue(DtsObject & d, int & outchan) {
  if (outputq.empty()) {
    return SMACQ_NONE;
  }
  
  d = outputq.front().first;
  outchan = outputq.front().second;
  
  outputq.pop_front();
  
  if (outputq.empty()) {
    return SMACQ_PASS;
  } else {
    //fprintf(stderr, "returning SMACQ_PASS|SMACQ_PRODUCE\n");
    return SMACQ_PASS|SMACQ_PRODUCE;
  }
}

struct smacq_functions {
  SmacqModule::constructor_fn * constructor;
  struct SmacqModule::algebra algebra;
/* Put constructor and algebra first so that we can use partial initializers in g++ */
};

#else
typedef void SmacqModule;
#endif

#endif	


