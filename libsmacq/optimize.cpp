#include <assert.h>
#include <SmacqGraph.h>

/// Recursively initialize a list of all the tails in this given graph
inline void SmacqGraph::list_tails(std::deque<SmacqGraph*> &list) {
  SmacqGraph * b = this->find_branch();

  if (b) {
    FOREACH_CHILD(b, child->list_tails(list));
  } else if (this->children[0].size()) {
    // not interested in tails that are heads, but we will take their children
    return this->children[0][0]->list_tails(list);
  } else {
    // base case: found a tail
    list.push_back(this);
  }
}

inline bool SmacqGraph::compare_element_names(SmacqGraph * a, SmacqGraph * b) {
  return !strcmp(a->argv[0], b->argv[0]);
}
  
/// true iff graphs are equivalent 
inline bool SmacqGraph::equiv(SmacqGraph * a, SmacqGraph * b) {
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

inline void SmacqGraph::merge_tails() {
  SmacqGraph * bp;
  std::deque<SmacqGraph*> list;
  std::deque<SmacqGraph*>::iterator lpa, lpb;

  /* Get all the tails we have */
  for (bp = this; bp; bp=bp->next_graph) {
    bp->list_tails(list);
  }

  /* n**2 comparison between each tails */
  for (lpa = list.begin(); lpa != list.end(); lpa++) {
    for (lpb = lpa; lpb != list.end(); lpb++) {
      if (*lpa != *lpb && equiv(*lpa, *lpb)) {
	fprintf(stderr, "tails %p and %p merging\n", *lpa, *lpb);
	while ((*lpb)->numparents) {
	  (*lpb)->parent[0]->replace_child(*lpb, *lpa);
	}

	// lpb is now unreferenced
	delete *lpb;

	list.erase(lpb);

	merge_tails(); // iterators hosed; start over
	return;

      }
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

inline void SmacqGraph::move_children(SmacqGraph * from, SmacqGraph * to, bool addvector) {
    FOREACH_CHILD(from, {
	/// If the module is vectorized, then add a new vector element;
	/// otherwise add it to channel 0
	if (addvector) {
	  to->add_child(child, to->children.size());
	} else {
	  to->add_child(child);
	}
	from->remove_child(i,j);
	j--; //Fixup iterator
      });
}
									 
/// Merge these two nodes if possible.
/// The nodes should have the same inputs (including no inputs).
/// Return true iff b is eliminated.
inline bool SmacqGraph::merge_nodes(SmacqGraph * a, SmacqGraph * b) {
  if ( (a==b) // Can't merge self with self
       // Can only merge stateless
       || !a->algebra.stateless || !b->algebra.stateless 
       // If the name's not event the same, forget it
       || !compare_element_names(a,b)
       ) 
    {
      return false;
    }

  if (a->algebra.vector && b->algebra.vector) {
    // Move all children (including arguments) from B to A 
    a->add_args(b);
    move_children(b, a, true);
    return true;
  } else if (equiv(a,b)) {
    // Move all children
    move_children(b, a);
    return true;
  } else {
    return false;
  }
}

/// Examine children and remove any stateless ones.
/// Their children will be combined.
void SmacqGraph::merge_redundant_children() {
  for (unsigned int k = 0; k < children.size(); k++) {
    for (unsigned int i = 0; i < children[k].size(); i++) {
      SmacqGraph * twina = children[k][i];
      // Look for twins among siblings
      for (unsigned int j = 0; j < children[k].size(); j++) {
	SmacqGraph * twinb = children[k][j];	
	if (merge_nodes(twina, twinb)) {
	  // Remove twin 
	  remove_child(k,j);
	  
	  // This invalidates iterators, so crudely start over
	  return merge_redundant_children();
	}
      }
      
      // Recurse
      children[k][i]->merge_redundant_children();
    }
  }
  
  if (next_graph) next_graph->merge_redundant_children();
}

inline bool SmacqGraph::same_children(SmacqGraph *a, SmacqGraph *b) {
  if (a->children.size() < b->children.size()) {
    // Swap so that a is >= b in size
    SmacqGraph * g = a;
    a = b; b = g;
  }
    
  // This works even if a->children.size() is larger, but sparse 
  for (unsigned int i = 0; i < a->children.size(); i++) {
    if (i >= b->children.size() 
	|| a->children[i].size() != a->children[i].size()) {
      return false;
    }
    for (unsigned int j = 0; j < a->children.size(); j++) {
      if (a->children[i][j] != b->children[i][j]) {
	return false;
      }
    }
  }
  return true;
}

/// Examine stateless nodes and merge any duplicate parents.
/// Return true if something was done (recursively)
bool SmacqGraph::merge_redundant_parents() {
  // Recurse first so we do this bottom up
  FOREACH_CHILD(this, {
      if (child->merge_redundant_parents()) return true;
    });
  if (next_graph) {
    while (next_graph->merge_redundant_parents()) ;
  }

  for (int i = 0; i < numparents; i++) {
    if (parent[i]->algebra.stateless) {
      for (int j = i+1; j < numparents; j++) {
	if (parent[i] != parent[j] && equiv(parent[i], parent[j]) && 
	    same_children(parent[i], parent[j])) {

	  //fprintf(stderr, "merging redundant parents %p and %p\n", parent[i], parent[j]);

	  // Tell grandparents to replace j
	  SmacqGraph * departing = parent[j];
	  while (departing->numparents) {
	    assert(departing->parent[0] != parent[i]);
	    departing->parent[0]->add_child(parent[i]);
	    departing->parent[0]->remove_child(departing);
	  }
	  
	  // Now that nothing will come from j, we can remove it
	  FOREACH_CHILD(departing, {
	      assert(child);  // Remove compiler warning
	      departing->remove_child(i,j);
	      j--; // Fixup iterator
	    });
	    
	  // Free j
	  assert(departing->numparents == 0);
	  FOREACH_CHILD(departing, assert(!child));
	  //delete departing;

	  return true; // Iterators hosed, so ask to be restarted
	}
      }
    }
  }

  return false;
}

void SmacqGraph::merge_heads() {
#ifndef SMACQ_OPT_NOHEADS
  SmacqGraph * ap, * bp;

  /* Merge identical heads */
  for (ap = this; ap ; ap = ap->next_graph) {
    SmacqGraph * bprev = ap;
    for (bp = ap->next_graph; bp; bp=bp->next_graph) {
      if (merge_nodes(ap, bp)) {
	// Remove bp from list
	bprev->next_graph = bp->next_graph;\

	// Fixup iterator
	bp = bprev;
      }
    }
  }
#else
#warning "SMACQ_OPT_NOHEADS set"
#endif
}

void SmacqGraph::optimize() {
#if defined(SMACQ_DEBUG2) && !defined(SMACQ_NO_OPT)
  print(stderr, 8);
#endif

  merge_heads();

  /* Do the merge again from the tails up */
  merge_tails();

  // From the bottoms up, identify redundant parents
  while (merge_redundant_parents()) ;

  // From the top down, identify redundant (twin) children
  merge_redundant_children();

#ifdef SMACQ_DEBUG
  fprintf(stderr, "--- final is ---\n");
  print(stderr, 8);
#endif
}
