#ifndef SMACQ_PRODUCEQ_C
#define SMACQ_PRODUCEQ_C

struct smacq_outputq {
	DtsObject * o;
	int outchan;
	struct smacq_outputq * next;
	struct smacq_outputq * end;
};

static inline DtsObject * smacq_produce_peek(struct smacq_outputq ** qp) {
  if (*qp) {
  	return((*qp)->o);
  } else { 
	return NULL;
  }
}
  
static inline void smacq_produce_enqueue(struct smacq_outputq ** qp, DtsObject * o, int outchan) {
  struct smacq_outputq * nq = (struct smacq_outputq*)malloc(sizeof(struct smacq_outputq));
  assert(nq);
  assert(qp);

  nq->o = o;
  nq->outchan = outchan;
  nq->next = NULL;

  if (!*qp) {
  	  //fprintf(stderr, "q %p as head\n", nq);
	  *qp = nq;
  } else {
  	  //fprintf(stderr, "q %p after %p\n", nq, (*qp)->end);
	  (*qp)->end->next = nq;
  }

  (*qp)->end = nq;
}

static inline smacq_result smacq_produce_canproduce(struct smacq_outputq ** qp) {
  if (*qp) {
	  return SMACQ_PRODUCE;
  } else {
	  return (smacq_result)0;
  }
}

static inline smacq_result smacq_produce_dequeue(struct smacq_outputq ** qp, DtsObject ** o, int * outchan) {
  struct smacq_outputq * head = *qp;

  if (!head) {
	  return SMACQ_FREE;
  }

  *o = head->o;
  *outchan = head->outchan;
  *qp = head->next;

  /* Update both ends of queue */
  if (head->next) {
	head->next->end = head->end;
  } 

  /* fprintf(stderr, "popped %p: %p\n", head, head->o); */

  free(head);

  if (*qp) {
    /* fprintf(stderr, "next is %p: %p\n", *qp, (*qp)->o); */
	return (smacq_result)(SMACQ_PASS|SMACQ_PRODUCE);
  } else {
	return SMACQ_PASS;
  }
}



#endif
