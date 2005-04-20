#ifndef RUNQ_H
#define RUNQ_H
#include <DtsObject.h>

struct qel {
  SmacqGraph_ptr f;
  DtsObject d;
  struct qel * next;
#ifdef SMACQ_OPT_RUNRING
  struct qel * prev;
#endif
};
 	
class runq {
 public:
  runq();

  /// Remove the first element from the runq.  Set f & d accordingly.
  bool pop_runable(SmacqGraph_ptr & f, DtsObject &d);

  /// Add a new element to the end of the runq.
  void runable(SmacqGraph * f, DtsObject d);

  /// Return TRUE iff the graph is in this runq.
  bool find(SmacqGraph * f);

  /// Remove any instances of the graph in this runq. (expensive!)
  void remove(SmacqGraph * f);

  /// Print-out the runq.
  void print(FILE * fh = stderr);

 protected:
  /// Create a new blank entry in the queue
  struct qel * insert_before(struct qel *);

  /// Return a pointer to the current tail (which can be filled) and update to new tail.
  inline struct qel * insertion_point(SmacqGraph * f);

  struct qel * head;
  struct qel * tail;
};

#include <SmacqGraph.h>

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

inline struct qel * runq::insertion_point(SmacqGraph * f) {
  struct qel * el = tail;

  /* Got to tail */
  tail = tail->next;
  
  /* if the new tail would stomp on the head, then make some more room */
  if (tail == head) {
    tail = insert_before(head);
  }

  return el;
}

void inline runq::remove(SmacqGraph * f) {
  if (!head) return;

  if (head->f == f) {
	head = head->next;
	if (head == tail) {
		head = NULL;
		return;
	}
  }
  struct qel * prev = head;

  for (struct qel * el = head->next; el != tail; el = el->next) {
	if (el->f == f) {
		if (el->next == tail) {
			tail = el;
		} else {
			// Shrinking isn't very graceful, but best option?
			prev->next = el->next;
		}
		delete el;
		el = prev; // Fixup iterator
	} else {
		prev = el; 
	}
  }
}

bool inline runq::find(SmacqGraph * f) {
  if (!head) return false;

  for (struct qel * el = head; el != tail; el = el->next) {
	if (el->f == f) return true;
  }
  return false;
}

/// Print the contents of the runq.
void inline runq::print(FILE * fh) {
  fprintf(fh, "Runq:");
  if (head) {
    for (struct qel * el = head; el != tail; el = el->next) {
      fprintf(fh, "\t-> %p, %p\n", el->f.get(), el->d.get());
    }
  }
  fprintf(fh, "\n");
}

/// Add something to the runq.
void inline runq::runable(SmacqGraph * f, DtsObject d) {
  struct qel * el = insertion_point(f);

  el->f = f;
  el->d = d;
		
  //fprintf(stderr, "%p now runable\n", el->f);
  
  if (!this->head) {
    this->head = el;
  }

  //this->print();
}

inline bool runq::pop_runable(SmacqGraph_ptr & f, DtsObject &d) {
  if (!this->head) {
    //fprintf(stderr, "queue %p/%p empty\n", this, this->head);
    return false;
  }

  f = this->head->f;
  d = this->head->d;

  //fprintf(stderr, "%p for %p off queue from %p/%p\n", this->head->d, this->head->f, this, this->head);

  this->head->f = NULL;
  this->head->d = NULL;

  this->head = this->head->next;

  if (this->head == this->tail) {
    /* Ring is empty */
    this->head = NULL;
  }
  return true;
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

#endif
