#include <assert.h>
#include <SmacqGraph.h>
#include <string>

inline bool SmacqGraphNode::compare_element_names(SmacqGraphNode * a, SmacqGraphNode * b) {
  return !strcmp(a->argv[0], b->argv[0]);
}
  
/// true iff graphs are equivalent 
inline bool SmacqGraphNode::equiv(SmacqGraphNode * a, SmacqGraphNode * b) {
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
  std::set<SmacqGraphNode*> list;
  std::set<SmacqGraphNode*>::iterator lpa, lpb;
  list_tails(list);

  /* n**2 comparison between each tails */
  for (lpa = list.begin(); lpa != list.end(); ++lpa) {
    for (lpb = lpa; lpb != list.end(); ++lpb) {
      if (*lpa != *lpb && SmacqGraphNode::equiv(*lpa, *lpb)) {
	//fprintf(stderr, "tails %p and %p merging\n", *lpa, *lpb);
	while ((*lpb)->parent.size()) {
	  (*lpb)->parent[0]->replace_child(*lpb, *lpa);
	}

	merge_tails(); // iterators hosed; start over
	return;

      }
    }
  }
}

inline void SmacqGraphNode::add_args(SmacqGraphNode * b) {
  int i;
  this->argv = (char**)realloc(this->argv, (this->argc + b->argc) * sizeof(char*));
  this->argv[this->argc++] = ";";
  for (i=1; i < b->argc; i++) {
    this->argv[this->argc++] = b->argv[i];
  }
}

inline void SmacqGraphNode::move_children(SmacqGraphNode * from, SmacqGraphNode * to, bool addvector) {
    FOREACH_CHILD(from, {
	/// If the module is vectorized, then add a new vector element;
	/// otherwise add it to channel 0
	if (addvector) {
	  to->add_child(child, to->children.size());
	} else {
	  to->add_child(child);
	}
	from->remove_child_bynum(i,j);
	j--; //Fixup iterator
      });
}
									 
/// Merge these two nodes if possible.
/// The nodes should have the same inputs (including no inputs).
/// Return true iff b is eliminated.
inline bool SmacqGraphNode::merge_nodes(SmacqGraphNode * a, SmacqGraphNode * b) {
  if ( (a==b) // Can't merge self with self
       // Can only merge stateless
       || !a->algebra.stateless || !b->algebra.stateless 
       // If the name's not even the same, forget it
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
void SmacqGraphNode::merge_redundant_children() {
  for (unsigned int k = 0; k < children.size(); k++) {
    for (unsigned int i = 0; i < children[k].size(); i++) {
      SmacqGraphNode * twina = children[k][i].get();
      // Look for twins among siblings
      for (unsigned int j = 0; j < children[k].size(); j++) {
	SmacqGraphNode * twinb = children[k][j].get();	
	if (merge_nodes(twina, twinb)) {
	  // Remove twin 
	  remove_child_bynum(k,j);
	  
	  // This invalidates iterators, so crudely start over
	  return merge_redundant_children();
	}
      }
      
      // Recurse
      children[k][i]->merge_redundant_children();
    }
  }
}

inline bool SmacqGraphNode::same_children(SmacqGraphNode * a, SmacqGraphNode * b) {
  if (a->children.size() < b->children.size()) {
    // Swap so that a is >= b in size
    SmacqGraphNode * g = a;
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
bool SmacqGraphNode::merge_redundant_parents() {
  // Recurse first so we do this bottom up
  FOREACH_CHILD(this, {
      if (child->merge_redundant_parents()) return true;
    });

  for (unsigned int i = 0; i < parent.size(); i++) {
    if (parent[i]->algebra.stateless) {
      for (unsigned int j = i+1; j < parent.size(); j++) {
	if (parent[i] != parent[j] && equiv(parent[i], parent[j]) && 
	    same_children(parent[i], parent[j])) {

	  //fprintf(stderr, "merging redundant parents %p and %p\n", parent[i], parent[j]);

	  SmacqGraphNode_ptr departing = parent[j];

	  // Tell grandparents to replace j
	  while (departing->parent.size()) {
	    assert(departing->parent[0] != parent[i]);
	    departing->parent[0]->add_child(parent[i]);
	    departing->parent[0]->remove_child(departing.get());
	  }
	  
	  departing->remove_children();

	  return true; // Iterators hosed, so ask to be restarted
	}
      }
    }
  }

  return false;
}

class GraphVector : public std::vector<SmacqGraphNode*> {
  public:
	void erase(unsigned int i) {
	   // Swap, drop, and roll...
	   unsigned int last = size() - 1;
	   if (i < last) 
	   	(*this)[i] = (*this)[last];
	   (*this)[last] = NULL;
	   resize(last);
	}
};

void SmacqGraph::merge_heads() {
#ifndef SMACQ_OPT_NOHEADS
  /* Merge identical heads */
  for (unsigned int i = 0; i < head.size(); i++) {
    for (unsigned int j = 0; j < head.size(); j++) {
      if (SmacqGraphNode::merge_nodes(head[i].get(), head[j].get())) {
	// Remove j from list
	head.erase(j);

	// Fixup iterator
	j--;
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

  //fprintf(stderr, "Before optimize: \n");
  //this->print(stderr, 15);

  merge_heads();

  //fprintf(stderr, "Merged heads\n");
  //this->print(stderr, 15);

  /* Do the merge again from the tails up */
  merge_tails();

  //fprintf(stderr, "Merged tails\n");
  //this->print(stderr, 15);

  // From the bottoms up, identify redundant parents
  for (unsigned int i = 0; i < head.size(); i++) {
    while (head[i]->merge_redundant_parents()) ;
  }

  //fprintf(stderr, "Merged parents\n");
  //this->print(stderr, 15);

  // From the top down, identify redundant (twin) children
  for (unsigned int i = 0; i < head.size(); i++) {
  	head[i]->merge_redundant_children();
  }

  //fprintf(stderr, "Merged children\n");
  //this->print(stderr, 15);

#ifdef SMACQ_DEBUG
  fprintf(stderr, "--- final is ---\n");
  print(stderr, 8);
#endif
}

/// Recursive function to find best point at which to distribute graph.
/// Return NULL iff there is nothing further to distribute.
/// Otherwise, return pointer to loation of recollection point to replace us with.
SmacqGraph * SmacqGraphNode::distribute_rejoin() {
  if (!algebra.stateless) {
	return NULL;
  }

  // Base case
  if (children.size() == 1 && children[0].size() == 0) {
	return NULL;
  }

  // XXX. only handles out-degree = 1
  if (children.size() == 1 && children[0].size() == 1) {
	// See if the barrier is further down
	SmacqGraph * b = children[0][0]->distribute_rejoin();
	if (b) return b;
  }

  // If we get here, then we're at the barrer point.
  // This node will be distributed, but our children will be local
  //fprintf(stderr, "Distributing at node %p: %s\n", this, argv[0]);

  assert(children.size() == 1);

  // Make a new ref to children
  SmacqGraph * c = new SmacqGraph(children[0]);
  
  // Now safe to remove
  remove_children();

  return c;
} 

// Farm out children to run in parallel.
// May do nothing if unable identify candidate children.
// If return value is non-NULL, then it is ...
bool SmacqGraphNode::distribute_children(DTS * dts) {
  if (children.size() != 1) {
  	// XXX: We don't distribute vectors
	// Unsupported
	return false;
  }

  FOREACH_CHILD(this, {
  	// Get a pointer to the re-collection point in our children.
  	// As a side effect, this will move some of our descendents to the rejoin
	SmacqGraph * rejoin = child->distribute_rejoin();

        // See if there is anything to be distributed
	if (rejoin) {
		SmacqGraphNode_ptr c = child;
		remove_child_bynum(i, j);  j--;
		std::string q("distribute ");
		q += c->print_query();
		fprintf(stderr, "distribute %s\n", q.c_str());
  		SmacqGraph dist;
		dist.addQuery(dts, scheduler, q);
		dist.join(rejoin, true);
  		this->join(&dist, true);

		/*
		fprintf(stderr, "portion to distribute:\n");
		child->print(stderr, 40);
		fprintf(stderr, "my new children:\n");
		dist->print(stderr, 40);
		*/
	
	}
  });
  return true; // well, maybe
}
