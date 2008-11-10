#ifndef RUNQ_H
#define RUNQ_H
#include <DtsObject.h>
#include <ThreadSafe.h>

#ifdef USE_GASNET
# include <gasnet.h>
#endif

template<typename T>	
class runq : public PthreadMutex {
 public:
  runq() : head(NULL), qlen(0) {
    /* Make a 2 element ring to start */
    tail = new qel;
    tail->next = new qel;
    tail->next->next = tail;

    tail->next->prev = tail;
    tail->prev = tail->next;
  }

  bool empty() {
    return !this->head;
  }

  size_t size() {
    return qlen;
  }

  /// Copy the first element from the runq.  Set f & d accordingly.
  bool peek(T& val) {
    RECURSIVE_LOCK(this);

    if (!this->head) {
      return false;
    } else {
      val = this->head->val;
      return true;
    }
  }

  /// Remove the first element from the runq.  Set f & d accordingly.
  bool pop(T& val) {
    RECURSIVE_LOCK(this);

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

    qlen--;
    return true;
  }

  /// Add a new element to the end of the runq.  Return true iff this is only element on queue
  bool enqueue(T val) {
    RECURSIVE_LOCK(this);

    struct qel * el = insertion_point();

    el->val = val;
		
    qlen++;

    if (!this->head) {
      this->head = el;
      return true;
    }

    return false;
  }

  /// Print the contents of the runq.
  void print(FILE * fh) {
    RECURSIVE_LOCK(this);

    fprintf(fh, "Queue:");
    if (head) {
      for (struct qel * el = head; el != tail; el = el->next) {
	fprintf(fh, "\tslot %p -> %p\n", el, el->val.get());
      }
    }
    fprintf(fh, "\n");
  }

 protected:
#ifdef USE_GASNET
  void lock() { 
	gasnet_hold_interrupts(); 
  }

  void unlock() { 
	gasnet_resume_interrupts(); 
  }
#endif

  struct qel {
    T val;
    struct qel * next;
#ifdef SMACQ_OPT_RUNRING
    struct qel * prev;
#endif
  };

  qel * head;
  qel * tail;

  size_t qlen;

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

/*
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
*/

  bool find(T val) {
    if (!head) return false;

    for (struct qel * el = head; el != tail; el = el->next) {
      if (el->val == val) return true;
    }
    return false;
  }
};
#endif
