#include <smacq.h>
#include <dlfcn.h>
#include <setjmp.h>
#include <ucontext.h>

#define STACK_SIZE 65535

struct state {
  jmp_buf event_stack;
  jmp_buf loop_stack;
  ucontext_t loop_context;

  struct smacq_init * context;

  const dts_object * datum;
  smacq_result result;

  struct dts_list * product, * product_tail;
};

struct dts_list {
  const dts_object * d;
  int outchan;
  struct dts_list * next;
};

const dts_object * smacq_read(struct smacq_init * context) {
  struct state * state = context->state;

  int res = setjmp(state->loop_stack);
  if (!res) {
  	  //fprintf(stderr, "thread: smacq_read blocking for new data\n");
	  longjmp(state->event_stack, 1);
  } else if (res == 1) {
  	  //fprintf(stderr, "thread: smacq_read returning new data\n");
	  return state->datum;
  } else if (res == 2) { /* Shutdown */
  	  //fprintf(stderr, "thread: smacq_read returning EOF\n");
	  return NULL;
  }

  return NULL; /* Shouldn't get here */
}

int smacq_flush(struct smacq_init * context) {
  struct state * state = context->state;
  int res = setjmp(state->loop_stack);
  if (!res) 
	  longjmp(state->event_stack, 1);
  else 
	  return (res-1);

}

void smacq_decision(struct smacq_init * context, const dts_object * datum, smacq_result result) {
  struct state * state = context->state;
  state->result = result;
}

void smacq_write(struct state * state, dts_object * datum, int outchan) {
  struct dts_list * entry = g_new(struct dts_list, 1);
  entry->d = datum;
  entry->outchan = outchan;

  entry->next = state->product_tail;
  state->product_tail = entry;
}

void run_thread(struct smacq_init * context, smacq_thread_fn * thread_fn) {
  	struct state * state = context->state;
	state->result = thread_fn(context);
	longjmp(state->event_stack, 1);
}

/*********************************/

smacq_result smacq_thread_shutdown(struct state * state) {
  if (!setjmp(state->event_stack)) 
	  longjmp(state->loop_stack, 2);

  return SMACQ_END;
}

smacq_result smacq_thread_init(struct smacq_init * volatile_context) {
  struct state * state = g_new0(struct state, 1);
  void * stack = malloc(STACK_SIZE);

  volatile_context->state = state;
  
  state->context = memdup(volatile_context, sizeof(struct smacq_init));

  assert(state->context->thread_fn);

  /* Create a context with a separate stack for the thread. */
  getcontext(&state->loop_context);
  state->loop_context.uc_link = NULL; /* Doesn't matter; run_thread will longjmp back */
  state->loop_context.uc_stack.ss_sp = stack;
  state->loop_context.uc_stack.ss_size = STACK_SIZE;
  makecontext(&state->loop_context, (void (*) (void)) run_thread, 
		  2, state->context, state->context->thread_fn);

  if (!setjmp(state->event_stack)) {
	//fprintf(stderr, "Starting up new stack\n");
  	setcontext(&state->loop_context);
  }

  return 0;
}

smacq_result smacq_thread_consume(struct state * state, const dts_object * datum, int * outchan) {
  state->datum = datum;

  if (!setjmp(state->event_stack)) {
  	  //fprintf(stderr, "thread: consume jumping back to loop\n");
	  longjmp(state->loop_stack, 1);
  }

  return state->result;
}

smacq_result smacq_thread_produce(struct state * state, const dts_object ** datum, int *outchan) {
  smacq_result result;
  
  if (!state->product) {
    result = SMACQ_FREE;
  } else {
    struct dts_list * entry = state->product;
    *datum = entry->d;
    *outchan = entry->outchan;
    
    state->product = entry->next;
    if (state->product_tail == entry) {
      state->product_tail = NULL;
    }

    free(entry);
    
    result = (SMACQ_PASS |  (state->product ? SMACQ_PRODUCE : 0));
  }

  return result;
}
