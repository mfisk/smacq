#include <smacq.h>
#include <semaphore.h>
#include <dlfcn.h>

struct state {
  pthread_t thread;
  pthread_mutex_t produce_lock;
  pthread_cond_t no_product;
  sem_t newdata, consumed;

  const dts_object * datum;
  smacq_result result;

  struct dts_list * product, * product_tail;
};

struct dts_list {
  const dts_object * d;
  int outchan;
  struct dts_list * next;
};

const dts_object * smacq_read(struct flow_init * context) {
  struct state * state = context->state;
  // fprintf(stderr, "thread: smacq_read blocking for new data\n");
  sem_wait(&state->newdata);
  // fprintf(stderr, "thread: smacq_read returning new data\n");
  return state->datum;
}

void  smacq_flush(struct flow_init * context) {
  struct state * state = context->state;
  int empty;
  pthread_mutex_lock(&state->produce_lock);

  empty =!state->product;

  if (!empty) {
    pthread_cond_wait(&state->no_product, &state->produce_lock);
  }

  pthread_mutex_unlock(&state->produce_lock);
  return;
}

void smacq_decision(struct flow_init * context, const dts_object * datum, smacq_result result) {
  struct state * state = context->state;
  state->result = result;
  // fprintf(stderr,"thread: smacq_decision signalling data consumed \n");
  sem_post(&state->consumed);
  // fprintf(stderr, "thread: smacq_decision done\n");
}

void smacq_write(struct state * state, dts_object * datum, int outchan) {
  struct dts_list * entry = g_new(struct dts_list, 1);
  entry->d = datum;
  entry->outchan = outchan;

  pthread_mutex_lock(&state->produce_lock);
  entry->next = state->product_tail;
  state->product_tail = entry;
  pthread_mutex_unlock(&state->produce_lock);
}

/*********************************/

smacq_result smacq_thread_shutdown(struct state * state) {
  // XXX
  return SMACQ_END;
}


static void * smacq_thread_init_void(void * void_context) {
  struct flow_init * context = void_context;
  assert(context->thread_fn);
  return (void*)context->thread_fn(context);
}

smacq_result smacq_thread_init(struct flow_init * volatile_context) {
  struct flow_init * context;
  struct state * state = g_new0(struct state, 1);
  volatile_context->state = state;
  
  context = memdup(volatile_context, sizeof(struct flow_init));

  pthread_mutex_init(&state->produce_lock, NULL);
  sem_init(&state->newdata, 0, 0);
  sem_init(&state->consumed, 0, 0);
  pthread_cond_init(&state->no_product, NULL);
  
  pthread_create(&state->thread, NULL, smacq_thread_init_void, context);

  return 0;
}

smacq_result smacq_thread_consume(struct state * state, const dts_object * datum, int * outchan) {
  state->datum = datum;
  //printf(stderr,"master: new data available\n");
  sem_post(&state->newdata);

  //fprintf(stderr, "master: waiting for new data to be consumed\n");
  sem_wait(&state->consumed);
  //fprintf(stderr, "master: smacq_consume got consumed condition; returning\n");

  return state->result;
}

smacq_result smacq_thread_produce(struct state * state, const dts_object ** datum, int *outchan) {
  smacq_result result;
  
  pthread_mutex_lock(&state->produce_lock);
  
  if (!state->product) {
    result = SMACQ_FREE;
  } else {
    struct dts_list * entry = state->product;
    *datum = entry->d;
    *outchan = entry->outchan;
    
    state->product = entry->next;
    if (state->product_tail == entry) {
      pthread_cond_signal(&state->no_product);
      state->product_tail = NULL;
    }

    free(entry);
    
    result = (SMACQ_PASS |  (state->product ? SMACQ_PRODUCE : 0));
  }
  pthread_mutex_unlock(&state->produce_lock);

  return result;
}
