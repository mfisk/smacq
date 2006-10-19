#ifndef SMACQMODULE_IMPL_H
#define SMACQMODULE_IMPL_H
#include <SmacqModule-interface.h>
#include <DtsField.h>
#include <dts.h>
#include <SmacqScheduler.h>

/* Usage:

SMACQ_MODULE(foo,
private:
...
);

*/

#define PROTO_CONSUME() public: smacq_result consume(DtsObject, int&); private:
#define PROTO_PRODUCE() public: smacq_result produce(DtsObject&, int&); private:
#define PROTO_CTOR(name) public: name##Module::name##Module(smacq_init *); private:
#define PROTO_DTOR(name) public: name##Module::~name##Module(); private:

#define SMACQ_MODULE(name,defs...)		\
  class name##Module : public SmacqModule {	\
  private:					\
    defs					\
      };					\
  EXPORT_SMACQ_MODULE(name, MODULE_ALGEBRA)

#define SMACQ_MODULE_THREAD(name,defs...)				\
  class name##Module : public ThreadedSmacqModule {			\
  public:								\
    smacq_result thread(smacq_init* context);	\
    name##Module(smacq_init * context) : ThreadedSmacqModule(context) {} \
  private:								\
    defs								\
      };								\
  EXPORT_SMACQ_MODULE(name, MODULE_ALGEBRA)

#define MODULE_ALGEBRA {			\
  stateless: SMACQ_MODULE_IS_STATELESS,		\
      vector: SMACQ_MODULE_IS_VECTOR,		\
      annotation: SMACQ_MODULE_IS_ANNOTATION,	\
      demux: SMACQ_MODULE_IS_DEMUX }

#ifndef SMACQ_MODULE_IS_STATELESS
#define SMACQ_MODULE_IS_STATELESS 0
#endif
#ifndef SMACQ_MODULE_IS_VECTOR
#define SMACQ_MODULE_IS_VECTOR 0
#endif
#ifndef SMACQ_MODULE_IS_ANNOTATION
#define SMACQ_MODULE_IS_ANNOTATION 0
#endif
#ifndef SMACQ_MODULE_IS_DEMUX
#define SMACQ_MODULE_IS_DEMUX 0
#endif

#define EXPORT_SMACQ_MODULE(name, alg)			\
  SMACQ_MODULE_CONSTRUCTOR(name);			\
  struct smacq_functions smacq_##name##_table = {	\
    constructor: &name##_constructor,			\
    algebra: alg					\
  };

#define SMACQ_MODULE_CONSTRUCTOR(name)					\
  static SmacqModule * name##_constructor(smacq_init * context) { \
    return new name##Module(context);					\
  }				

inline bool SmacqModule::usesOtherFields(DtsField f) {
  return usesFields.otherEntry(f[0]);
}

inline DtsField SmacqModule::usesfield(char * name) {
  DtsField res = dts->requirefield(name);	
  usesFields[res[0]] = true;
  return res;	
}

inline bool SmacqModule::UsesArray::otherEntry(unsigned int f) const {
  DynamicArray<bool>::const_iterator i;
  unsigned int j;

  for (i = begin(), j=0; i != end(); ++i, ++j) {
    if (*i) {
      if (j != f) return true;
    }
  }
  return false;
}

inline SmacqModule::SmacqModule(smacq_init * context) 
  : dts(context->dts), scheduler(context->scheduler), self(context->self)
{}

inline SmacqModule::~SmacqModule() {}

inline smacq_result SmacqModule::consume(DtsObject datum, int & outchan) {
  return SMACQ_ERROR;
}

inline smacq_result SmacqModule::produce(DtsObject & datum, int & outchan) {
  return SMACQ_END;
}

inline void SmacqModule::enqueue(DtsObject & datum, int outchan) {
  scheduler->enqueue(self, datum, outchan);
}

#endif	


