#ifndef RUNQ_H
#define RUNQ_H
#include <DtsObject.h>

template<typename T>	
class runq {
 public:
  runq() {
    /* Make a 2 element ring to start */
    tail = new qel;
    tail->next = new qel;
    tail->next->next = tail;

    tail->next->prev = tail;
    tail->prev = tail->next;

    head = NULL;
  }

  bool empty() {
    return !this->head;
  }

  /// Copy the first element from the runq.  Set f & d accordingly.
  bool peek(T& val) {
    if (!this->head) {
      return false;
    }
    val = this->head->val;
    return true;
  }

  /// Remove the first element from the runq.  Set f & d accordingly.
  bool pop(T& val) {
    if (!this->head) {
      //fprintf(stderr, "queue %p/%p empty\n", this, this->head);
      return false;
    }

    val = this->head->val;
    this->head->val = NULL;  // Need to empty in case reference counted

    this->head = this->head->next;

    if (this->head == this->tail) {
      /* Ring is empty */
      this->head = NULL;
    }
    return true;
  }

  /// Add a new element to the end of the runq.
  void enqueue(T val) {
    struct qel * el = insertion_point();

    el->val = val;
		
    if (!this->head) {
      this->head = el;
    }
  }

  /// Print the contents of the runq.
  void print(FILE * fh) {
    fprintf(fh, "Runq:");
    if (head) {
      for (struct qel * el = head; el != tail; el = el->next) {
	fprintf(fh, "\t-> %x\n", el->val);
      }
    }
    fprintf(fh, "\n");
  }

 protected:

  struct qel {
    T val;
    struct qel * next;
#ifdef SMACQ_OPT_RUNRING
    struct qel * prev;
#endif
  };

  qel * head;
  qel * tail;

  /* Use a ring of runq elements.  New elements are allocated as necesssary, but
   * we never shrink.  Most of the time we can reuse existing elements. 
   */

  /// Create a new blank entry in the queue
  qel * insert_before(qel * point) {
    /* Insert new element before head */
    qel * entry = new qel;
  
    entry->prev = point->prev;
    point->prev->next = entry;
  
    entry->next = point;
    point->prev = entry;

    return entry;
  }

  /// Return a pointer to the current tail (which can be filled) and update to new tail.
  struct qel * insertion_point() {
    struct qel * el = tail;

    /* Got to tail */
    tail = tail->next;
  
    /* if the new tail would stomp on the head, then make some more room */
    if (tail == head) {
      tail = insert_before(head);
    }

    return el;
  }

  void erase(T val) {
    if (!head) return;

    if (head->val == val) {
      head = head->next;
      if (head == tail) {
	head = NULL;
	return;
      }
    }
    struct qel * prev = head;

    for (struct qel * el = head->next; el != tail; el = el->next) {
      if (el->val == val) {
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

  bool find(T val) {
    if (!head) return false;

    for (struct qel * el = head; el != tail; el = el->next) {
      if (el->val == val) return true;
    }
    return false;
  }


};
#endif
