#include <cmalloc.h>

struct list {
  struct list_element * head;
  struct list_element * tail;

  struct cmalloc * cm_elements;
};

struct list_element {
  struct list_element * prev;
  struct list_element * next;
  void * data;
};

static inline void list_append_element(struct list * l, struct list_element * next) {
  next->prev = l->tail;
  next->next = NULL;

  if (l->head) {
	  l->tail->next = next;
	  l->tail = next;
  } else {
	  l->head = next;
	  l->tail = next;
  }
}

static inline void list_init(struct list * l) {
	l->head = NULL;
	l->tail = NULL;
	
	l->cm_elements = cmalloc_init(0,0);
}

static inline struct list_element * list_element_new(struct list * l, void * val) {
  struct list_element * el = cm_new(l->cm_elements, struct list_element, 1);
  el->next = NULL;
  el->prev = NULL;
  el->data = val;
  return el;
}

static inline void list_element_free(struct list * l, struct list_element * el) {
 cmfree(l->cm_elements, el);
}

static inline struct list_element * list_append_value(struct list * l, void * val) {
  struct list_element * el = list_element_new(l, val);
  list_append_element(l, el);
  return el;
}

static inline void list_remove_element(struct list * l, struct list_element * el) {
  if (el->prev) el->prev->next = el->next;
  if (el->next) el->next->prev = el->prev;
  if (l->head == el) l->head = el->next;
  if (l->tail == el) l->tail = el->prev;
}

static inline void list_free(struct list * l) {
	while (l->head) {
		struct list_element * el = l->head;
		l->head = l->head->next;
		list_element_free(l, el);
	}
	l->tail = NULL;
}

static inline void * list_peek_value(struct list * l) {
	if (! l->head) return NULL;
	return l->head->data;
}

static inline struct list_element * list_pop_element(struct list * l) {
	struct list_element * ret = l->head;
	list_remove_element(l, l->head);
	return ret;
}
  
