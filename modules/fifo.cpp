#include <stdlib.h>
#include <assert.h>
#include <smacq.h>

/* Programming constants */

static struct smacq_options options[] = {
  {NULL, {string_t:NULL}, NULL, 0}
};

struct obj_list{
  DtsObject * obj;
  struct obj_list * next;
};

SMACQ_MODULE(fifo,
  PROTO_CTOR(fifo);
  PROTO_CONSUME();
  PROTO_PRODUCE();

  struct obj_list * fifo;
  struct obj_list * last;
); 

fifoModule::fifoModule(smacq_init * context) : SmacqModule(context) { ; }

smacq_result fifoModule::consume(DtsObject * datum, int * outchan) {
  struct obj_list * newo = g_new(struct obj_list, 1);
  newo->obj = (DtsObject*)datum;
  newo->next = NULL;

  if (fifo) {
	  last->next = newo;
  } else {
	  fifo = newo;
  }
  last = newo;
  datum->incref();
  return (smacq_result)(SMACQ_FREE|SMACQ_CANPRODUCE);
}

smacq_result fifoModule::produce(DtsObject ** datum, int * outchan) {
  if (fifo) {
    struct obj_list * old = fifo;
    fifo = fifo->next;

    *datum = old->obj;
    free(old);
  } else {
    return(SMACQ_END);
  }
  return (smacq_result)(SMACQ_PASS|(fifo ? SMACQ_CANPRODUCE : 0));
}

