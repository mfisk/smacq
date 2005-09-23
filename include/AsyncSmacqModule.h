#include <SmacqModule-interface.h>
#include <setjmp.h>
#include <signal.h> /* Needed on MacOS for ucontext.h */
#include <ucontext.h>

/// A virtual base clase for SMACQ modules that are executed with in their own
/// "thread" instead of being event-driven.  This is typically easier
/// to program, but less efficient than a regular SmacqModule.  The
/// only method that should be implemented by a subclass of
/// ThreadedSmacqModule is thread().

class ThreadedSmacqModule : public SmacqModule	{
 public:
  smacq_result produce(DtsObject, int&);
  smacq_result consume(DtsObject, int&);
  ThreadedSmacqModule(smacq_init *);
  ~ThreadedSmacqModule();

  friend void run_thread(int args, ThreadedSmacqModule * ths) {
      ths->thread(ths->context);
  }

 protected: 

  /// This is the only method that subclasses should (and must)
  /// implement.  It performs all of the work of the module and uses
  /// the following methods to produce and consume data.  This
  /// function should not return until the module is completely
  /// finished.  Return SMACQ_END or SMACQ_ERROR.
  virtual smacq_result thread(smacq_init * context) = 0;

  /// @name Methods used by the thread() implementation
  /// @{

  /// Read a new data object to process.  
  DtsObject smacq_read();

  int smacq_flush();

  /// Register a decision regarding an input object
  void smacq_decision(DtsObject datum, smacq_result result);

  /// Produce a new object. 
  void smacq_write(DtsObject datum, int outchan);

  /// @}
 private:
  jmp_buf event_stack;
  jmp_buf loop_stack;
  ucontext_t loop_context;

  struct SmacqModule::smacq_init * context;

  DtsObject datum;
  smacq_result result;

  struct dts_list * product, * product_tail;
};

inline void ThreadedSmacqModule::smacq_decision(DtsObject datum, smacq_result r) {
  result = r;
}

