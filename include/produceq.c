#ifndef SMACQ_PRODUCEQ_C
#define SMACQ_PRODUCEQ_C

struct smacq_outputq {
	const dts_object * o;
	int outchan;
	struct smacq_outputq * next;
	struct smacq_outputq * end;
};

static inline void smacq_produce_enqueue(struct smacq_outputq ** qp, const dts_object * o, int outchan) {
  struct smacq_outputq * nq = malloc(sizeof(struct smacq_outputq));
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

static inline smacq_result smacq_produce_dequeue(struct smacq_outputq ** qp, const dts_object ** o, int * outchan) {
  struct smacq_outputq * head = *qp;

  if (!head) {
	  return SMACQ_FREE;
  }

  *o = head->o;
  *outchan = head->outchan;
  *qp = head->next;

  if (head->next) {
  	  (*qp)->end = head->end;
  }

  free(head);

  //fprintf(stderr, "popped %p leaving %p\n", head, *qp);

  if (*qp) {
	  return SMACQ_PASS|SMACQ_PRODUCE;
  } else {
	  return SMACQ_PASS;
  }
}



#endif
