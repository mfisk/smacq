#include <smacq.h>
//#include <dlfcn.h>
#include <ThreadedSmacqModule.h>

#define STACK_SIZE 65535

#ifdef __APPLE__
#define setcontext(a) assert(!"context switching not supported on MacOS")
#define getcontext(a) assert(!"context switching not supported on MacOS")
#define makecontext(a...) assert(!"context switching not supported on MacOS")
#endif

struct dts_list {
  DtsObject d;
  int outchan;
  struct dts_list * next;
};

DtsObject ThreadedSmacqModule::smacq_read() {
  int res = setjmp(this->loop_stack);
  if (!res) {
  	  //fprintf(stderr, "thread: smacq_read blocking for new data\n");
	  longjmp(this->event_stack, 1);
  } else if (res == 1) {
  	  //fprintf(stderr, "thread: smacq_read returning new data\n");
	  return this->datum;
  } else if (res == 2) { /* Shutdown */
  	  //fprintf(stderr, "thread: smacq_read returning EOF\n");
	  return NULL;
  }

  return NULL; /* Shouldn't get here */
}

int ThreadedSmacqModule::smacq_flush() {
  int res = setjmp(loop_stack);
  if (!res) 
	  longjmp(event_stack, 1);
  else 
	  return (res-1);

}


void ThreadedSmacqModule::smacq_write(DtsObject datum, int outchan) {
  struct dts_list * entry = new dts_list;
  entry->d = datum;
  entry->outchan = outchan;

  entry->next = this->product_tail;
  this->product_tail = entry;
}

/*********************************/

ThreadedSmacqModule::~ThreadedSmacqModule() {
  if (!setjmp(this->event_stack)) 
	  longjmp(this->loop_stack, 2);
}

ThreadedSmacqModule::ThreadedSmacqModule(struct SmacqModule::smacq_init * volatile_context) : SmacqModule(volatile_context) {
  char * stack = (char*)malloc(STACK_SIZE);

  context = (smacq_init*)memdup(volatile_context, sizeof(struct SmacqModule::smacq_init));

  /* Create a context with a separate stack for the thread. */
  getcontext(&this->loop_context);
  this->loop_context.uc_link = NULL; /* Doesn't matter; run_thread will longjmp back */
  this->loop_context.uc_stack.ss_sp = stack;
  this->loop_context.uc_stack.ss_size = STACK_SIZE;
  makecontext(&this->loop_context, (void (*) (void)) run_thread, 
		  1, this);

  if (!setjmp(this->event_stack)) {
	//fprintf(stderr, "Starting up new stack\n");
  	setcontext(&this->loop_context);
  }
}

smacq_result ThreadedSmacqModule::consume(DtsObject datum, int & outchan) {
  this->datum = datum;

  if (!setjmp(this->event_stack)) {
  	  //fprintf(stderr, "thread: consume jumping back to loop\n");
	  longjmp(this->loop_stack, 1);
  }

  return this->result;
}

smacq_result ThreadedSmacqModule::produce(DtsObject datum, int & outchan) {
  smacq_result result;
  
  if (!this->product) {
    result = SMACQ_FREE;
  } else {
    struct dts_list * entry = this->product;
    datum = entry->d;
    outchan = entry->outchan;
    
    this->product = entry->next;
    if (this->product_tail == entry) {
      this->product_tail = NULL;
    }

    free(entry);
    
    result = (smacq_result)(SMACQ_PASS |  (this->product ? SMACQ_PRODUCE : 0));
  }

  return result;
}
