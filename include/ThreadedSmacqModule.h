#include <smacq.h>
#include <setjmp.h>
#include <signal.h> /* Needed on MacOS for ucontext.h */
#include <ucontext.h>

class ThreadedSmacqModule : public SmacqModule	{
 public:
  smacq_result produce(DtsObject, int*);
  smacq_result consume(DtsObject, int*);
  ThreadedSmacqModule(smacq_init *);
  ~ThreadedSmacqModule();
  friend void run_thread(int args, ThreadedSmacqModule * ths) {
      ths->thread(ths->context);
  }

 protected: 
  DtsObject smacq_read();
  int smacq_flush();
  void smacq_decision(DtsObject datum, smacq_result result);
  void smacq_write(DtsObject datum, int outchan);

  virtual smacq_result thread(smacq_init * context) = 0;

 private:
  jmp_buf event_stack;
  jmp_buf loop_stack;
  ucontext_t loop_context;

  struct smacq_init * context;

  DtsObject datum;
  smacq_result result;

  struct dts_list * product, * product_tail;
};

inline void ThreadedSmacqModule::smacq_decision(DtsObject datum, smacq_result r) {
  result = r;
}

