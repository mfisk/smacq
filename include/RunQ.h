#include <DtsObject.h>

enum action { PRODUCE=1, SHUTDOWN=2, LASTCALL=3, CONSUME=4 };

struct qel {
  enum action action;
  SmacqGraph * f;
  DtsObject d;
  struct qel * next;
#ifdef SMACQ_OPT_RUNRING
  struct qel * prev;
#endif
};
 	
class runq {
 public:
  bool is_empty();
  runq();
  int pop_runable(SmacqGraph * & f, DtsObject &d, enum action & action);
  void runable(SmacqGraph * f, DtsObject d, enum action action);
  bool pending_normal(SmacqGraph * f);
  bool pending(SmacqGraph * f);

 protected:
  struct qel * insert_before(struct qel *);
  inline struct qel * insertion_point(SmacqGraph * f, enum action action);

  struct qel * head;
  struct qel * tail;
#ifndef SMACQ_OPT_RUNRING
  int empty;
#endif
};

/* Use a ring of runq elements.  New elements are allocated as necesssary, but
 * we never shrink.  Most of the time we can reuse existing elements. 
 */

inline struct qel * runq::insert_before(struct qel * point) {
  /* Insert new element before head */
  struct qel * entry = new qel;
  
  entry->prev = point->prev;
  point->prev->next = entry;
  
  entry->next = point;
  point->prev = entry;

  return entry;
} 

inline struct qel * runq::insertion_point(SmacqGraph * f, enum action action) {
  struct qel * el;
	
  if ((action == PRODUCE) && this->head) {
    // Find an action for this element
    for (el = head; el != tail; el = el->next) {
      if (el->f == f) {
	if (el == head) {
		return (head = insert_before(el));
	} else {
		return insert_before(el);
  	}
      }
    }
  } else {
    el = tail;
  }

  /* Got to tail */
  tail = tail->next;
  
  /* if the new tail would stomp on the head, then make some more room */
  if (tail == head) {
    tail = insert_before(head);
  }

  return el;
}

void inline runq::runable(SmacqGraph * f, DtsObject d, enum action action) {
  struct qel * el = insertion_point(f, action);

  el->f = f;
  el->d = d;
  el->action = action;
		
  //fprintf(stderr, "%p now runable in %p/%p\n", this->tail->d, this, this->tail);
  
  if (!this->head) {
    this->head = el;
  }
}

inline int runq::pop_runable(SmacqGraph * & f, DtsObject &d, enum action & action) {
	if (!this->head) {
		//fprintf(stderr, "queue %p/%p empty\n", this, this->head);
		return 0;
	}

	f = this->head->f;
	action = this->head->action;
	d = this->head->d;

	//fprintf(stderr, "%p for %p off queue from %p/%p\n", this->head->d, this->head->f, this, this->head);

	this->head->f = NULL;
	this->head->d = NULL;

	this->head = this->head->next;

	if (this->head == this->tail) {
		/* Ring is empty */
		this->head = NULL;
	}
	return 1;
}

inline runq::runq() {
	/* Make a 2 element ring to start */
	tail = new qel;
	tail->next = new qel;
	tail->next->next = tail;

	tail->next->prev = tail;
	tail->prev = tail->next;

	head = NULL;
}

inline bool runq::is_empty() {
	if (!head) {
		return true;
	} else {
		return false;
	}
}

inline bool runq::pending_normal(SmacqGraph * f) {
	struct qel * q = this->head;
	while (q) {
		if ((q->action != SHUTDOWN) && (q->action != LASTCALL) && (q->f == f)) {
			return 1;
		}
		q = q->next;
#ifdef SMACQ_OPT_RUNRING
		if (q == this->tail) return 0;
#endif
	}
	return 0;
}

inline bool runq::pending(SmacqGraph * f) {
	struct qel * q = this->head;
	while (q) {
		if ((q->action != SHUTDOWN) && (q->f == f)) {
			return 1;
		}
		q = q->next;
#ifdef SMACQ_OPT_RUNRING
		if (q == this->tail) return 0;
#endif
	}
	return 0;
}

