#include <assert.h>
#include <SmacqGraph.h>

struct list {
  SmacqGraph * g;
  SmacqGraph * parent;
  int childi;
  int childj;
  struct list * prev;
  struct list * next;
};

/* Return a list of all the tails in the given graph (list=NULL initially) */
inline struct list * SmacqGraph::list_tails(struct list * list, SmacqGraph * parent, int childi, int childj) {
  /* A tail is the highest point in the tree that only has linear (fanout = 1) children */
  SmacqGraph * b = this->find_branch();

  if (b) {
    FOREACH_CHILD(b, list = child->list_tails(list, b, i, j));

    return list;
  } else {
    /* base case: no branches below here, so prepend self */
    struct list * newl = (struct list*)malloc(sizeof(struct list));
    newl->g = this;
    newl->parent = parent;
    newl->childi = childi;
    newl->childj = childj;

    newl->prev = NULL;
    newl->next = list;
    if (list) list->prev = newl;

    return newl;
  }
}

inline bool SmacqGraph::compare_element_names(SmacqGraph * a, SmacqGraph * b) {
  return !strcmp(a->argv[0], b->argv[0]);
}
  
/* 1 iff graphs are equivalent */
inline bool SmacqGraph::compare_elements(SmacqGraph * a, SmacqGraph * b) {
  int i;

  if (a == b) 
    return true;

  if (a->argc != b->argc) 
    return false;

  for (i=0; i < a->argc; i++) {
    if (strcmp(a->argv[i], b->argv[i])) 
      return false;
  }
  
  return true;
}

//// true iff tails are recursively equivalent
inline bool SmacqGraph::merge_tail_ends(SmacqGraph * a, SmacqGraph * b) {

  /* Base case */
  if ((!a->children[0].size()) &&
      (!b->children[0].size())) {
    return compare_elements(a,b);
  }

  /* Recurse to children */
  if (merge_tail_ends(a->children[0][0], b->children[0][0])) {
    if (compare_elements(a,b)) {
      return true;
    } else {
      /* This is the top of the commonality */
      /* a and b are not equivalent, but their children are */
      b->replace_child(0, 0, a->children[0][0]);

      return false;
    }
  } else {
    return false;
  }
}

inline void SmacqGraph::merge_tails(struct list * alist, struct list * blist) {
  SmacqGraph * a = alist->g;
  SmacqGraph * b = blist->g;
  int bchildi = blist->childi;
  int bchildj = blist->childj;
  SmacqGraph * bparent = blist->parent;
  SmacqGraph * p;
  int adepth = 0; 
  int bdepth = 0; 
  int i;
 
  if (a == b) return;

  /* Get tails of equal length */
  for (p = a; p->children[0].size(); p = p->children[0][0]) {
    adepth++;
  }
  
  for (p = b; p->children[0].size(); p = p->children[0][0]) {
    bdepth++;
  }
  
  if (bdepth > adepth) {
    for (i=0; i < (bdepth-adepth); i++) {
      bparent = b;
      bchildi = 0;
      bchildj = 0;
      b = b->children[0][0];
    }
  } else if (adepth > bdepth) {
    for (i=0; i < (adepth-bdepth); i++) {
      a = a->children[0][0];
    }
  } 

  if (a == b) return;

  if (merge_tail_ends(a, b)) {
    assert(bparent); /* else this would be a common head and already removed */

    //fprintf(stderr, "going to replace %p (child %d of %p) with %p\n", bparent->children[0][bchild], bchild, bparent, a);
    bparent->replace_child(bchildi, bchildj, a);
    
    /* Remove b tail from list of tails */
    /* XXX: Not freed because still used in current call stack */
    if (blist->prev) 
      blist->prev->next = blist->next;

    if (blist->next)
      blist->next->prev = blist->prev;

  }
}

inline void SmacqGraph::merge_all_tails() {
  struct list * list = NULL;
  struct list * lpa, * lpb;
  SmacqGraph * bp;

  /* Get all the tails we have */
  for (bp = this; bp; bp=bp->next_graph) {
    list = bp->list_tails(list, NULL, 0, 0);
  }

  /* n**2 comparison between each tails */
  for (lpa = list; lpa; lpa=lpa->next) {
    for (lpb = list; lpb; lpb=lpb->next) {
      if (lpa != lpb)
	merge_tails(lpa, lpb);
    }
  }
}

inline void SmacqGraph::add_args(SmacqGraph * b) {
  int i;
  this->argv = (char**)realloc(this->argv, (this->argc + b->argc) * sizeof(char*));
  this->argv[this->argc++] = ";";
  for (i=1; i < b->argc; i++) {
    this->argv[this->argc++] = b->argv[i];
  }
}

inline int SmacqGraph::merge_demuxs(SmacqGraph * a, SmacqGraph * b) {
  int match = 1;
  unsigned int i, j;

  if (!compare_elements(a,b)) return 0;

  if (a->children.size() != b->children.size()) return 0;

  /*
   * For demuxs, we can only merge the nodes if 
   * their immediate children are also identical and in same order 
   */
  for (i = 0; i < b->children.size(); i++) {
    if (a->children[i].size() != b->children[i].size())
      return 0;

    for (j = 0; j < b->children[i].size(); j++) {
      if (! compare_elements(a->children[i][j], b->children[i][j])) {
	match = 0;
	break;
      }
    }
  }
  if (match) {
    /* Okay, we know that a == b and a's children == b's children.
     * So B will be completely replaced by A.
     * Therefore, we don't need to do any recursive merging 
     */
    return 1; /* This will tell the caller to replace refs to b with a */
  } else {
    /* Can't merge A and B */
    return 0;
  }
}

inline int SmacqGraph::merge_vectors(SmacqGraph * a, SmacqGraph * b) {
#ifndef SMACQ_OPT_VECTORS
#warning "SMACQ_OPT_VECTORS not set"
  return 0;
#endif 

  if (!a->algebra.vector || 
      !b->algebra.vector || 
      !compare_element_names(a,b))
    return 0;
  
  /* Move all children (including arguments) from B to A */
  /* XXX: We could look at each arg in vector and treat as set instead of bag */

  //fprintf(stderr, "vector %p added to %p\n", b, a);
  a->add_args(b);

  /* Add children while in order */
  for (unsigned int i=0; i < b->children.size(); i++) {
    int ai = a->children.size();
    
    for (unsigned int j=0; j < b->children[i].size(); j++) {
      b->children[i][j]->remove_parent(b);
      a->add_child(b->children[i][j], ai);
      b->children[i][j] = NULL;
    }

    b->children[i].clear();
  }
  b->children.clear();
  
  return 1;
}

inline int SmacqGraph::merge_fanouts(SmacqGraph * a, SmacqGraph * b) {
  /*
   * For fanout nodes, we can just make the children of A be the union of 
   * the children of A and B.  Then get rid of B.
   * Note that this implementation also handles the case that the two sets are identical.
   */
  unsigned int i, j;
  
  if (!compare_elements(a,b)) return 0;

  assert(a->children.size() <= 1 && b->children.size() <= 1);;

  /* Move children selectively */
  for (i = 0; i < b->children[0].size(); i++) {
    /* If any of our twins children are twins of our children, just use that child */
    for (j = 0; j < a->children[0].size(); j++) {
      if (merge_trees(a->children[0][j], b->children[0][i])) {
	//fprintf(stderr, "%p will be handled by %p\n", b->children[0][i], a->children[0][j]);
	b->children[0][i] = NULL;
	break;
      }
    }
    
    /* Pawn this child off on our newly found twin */
    if (b->children[0][i]) {
      a->add_child(b->children[0][i]);
      /* fprintf(stderr, "%p child %p(%s) now child of %p(%s)\n", b, 
	 b->children[0][i], b->children[0][i]->name, 
	 a, a->name);  */
      b->children[0][i]->remove_parent(b);
    }
  }
  
  return 1;
}

/* Merge the tops of these two trees as much as possible */
/* Return 1 iff graph b was completely merged into a */
inline int SmacqGraph::merge_trees(SmacqGraph * a, SmacqGraph * b) {
  int retval;
  
  if (a==b)
    return 0;
  
  if (0 && merge_demuxs(a,b)) {
    retval = 1;
  } else if (a->algebra.vector) { 
    retval = merge_vectors(a,b);
  } else {
    retval = merge_fanouts(a, b);
  }
  
  return retval;
}

/* Search for mergeable children anywhere in tree */
inline void SmacqGraph::optimize_tree() {
  unsigned int an, bn;
  SmacqGraph * a, * b;
  
  if (!algebra.vector && !algebra.demux) { /* Can't rewrite children of a vector */
    for (an = 0; an < children[0].size(); an++) {
      a = children[0][an];
      
      for (bn=1; bn < children[0].size(); bn++) {
	b = children[0][bn];
	
	if (merge_trees(a, b)) {
	  //fprintf(stderr, "removed %p from %p\n", b, g);
	  b->remove_child(0, bn);
	  bn--; /* remove_child will reset children[0][j] to something new */
	}
      }
    }
  }
  
  for (an = 0; an < children[0].size(); an++) {
    children[0][an]->optimize_tree();
  }
  
  next_graph->optimize_tree();
}

void SmacqGraph::optimize() {
  SmacqGraph * ap, * prev;
  SmacqGraph * candidate;

#if defined(SMACQ_DEBUG2) && !defined(SMACQ_NO_OPT)
  print(stderr, 8);
#endif

#ifdef SMACQ_OPT_HEADS
  /* Merge identical heads */
  prev = this;
  for (candidate=prev->next_graph; candidate; candidate=prev->next_graph) {
    int merged = 0;
    SmacqGraph * next = candidate->next_graph;
    
    for (ap = this; ap; ap=ap->next_graph) {
      merged = merge_trees(ap, candidate);

      if (merged) {
	break;
      }
    }

    if (merged) {
      /* Remove candidate from the list */
      prev->next_graph = next;
    } else {
      /* Move down list */
      prev = prev->next_graph;
    }
  }


#ifdef SMACQ_DEBUG2
  fprintf(stderr, "--- merged heads (including vectors, demux, etc.) to ---\n");
  print(stderr, 8);
#endif
#else
#warning "SMACQ_OPT_HEADS not set"
#endif

#ifdef SMACQ_OPT_CHILDREN
  /* Now look for common leading subexpressions within trees */
  //optimize_tree();

#ifdef SMACQ_DEBUG2
  fprintf(stderr, "--- optimized internal heads to ---\n");
  print(stderr, 8);
#endif
#else
#warning "SMACQ_OPT_CHILDREN not set"
#endif

#ifdef SMACQ_OPT_TAILS
  /* Do the merge again from the tails up */
  merge_all_tails();

#ifdef SMACQ_DEBUG2
  fprintf(stderr, "--- optimized tails to ---\n");
  print(stderr, 8);
#endif
#else
#warning "SMACQ_OPT_TAILS not set"
#endif

#ifdef SMACQ_DEBUG
  fprintf(stderr, "--- final is ---\n");
  print(stderr, 8);
#endif
}
