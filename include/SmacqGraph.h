#ifndef SMACQ_GRAPH_H
#define SMACQ_GRAPH_H
#include <SmacqGraph-interface.h>
#include <IterativeScheduler-interface.h>

inline SmacqGraph::~SmacqGraph() {
  if (scheduler) {
	scheduler->do_shutdown(this);
  }
  children.clear();
  parent.clear();
  numparents = 0;
  next_graph = NULL;
}

/// Establish a parent/child relationship on the specified channel
inline void SmacqGraph::add_child(SmacqGraph * newo, unsigned int channel) {
  assert(newo != this);
  if (channel >= children.size()) {
    children.resize(channel+1);
  }
  children[channel].push_back(newo);
  newo->add_parent(this);
}

/// Recursively initialize a list of all the tails of this bag of graphs
inline void SmacqGraph::list_tails(std::set<SmacqGraph *> &list) {
  for(SmacqGraph * sg = this; sg; sg=sg->nextGraph()) {
  	sg->list_tails_recurse(list);
  }
}

/// Recursively initialize a list of all the tails in this given graph
inline void SmacqGraph::list_tails_recurse(std::set<SmacqGraph *> &list) {
  bool has_child = false;

  FOREACH_CHILD(this, {
	has_child = true;
	child->list_tails_recurse(list);
	});
  
  if (!has_child) {
    list.insert(this); // Will ignore dups
  }
}

inline void SmacqGraph::join(SmacqGraph * newg) {
  if (!newg) { return; }

  std::set<SmacqGraph *> list;
  std::set<SmacqGraph *>::iterator i;
  list_tails(list);

  for(i = list.begin(); i != list.end(); ++i) {
    for(SmacqGraph * sg = newg; sg; sg=sg->nextGraph()) {
      (*i)->add_child(sg); 
    }
  }
}

inline void SmacqGraph::add_graph(SmacqGraph * b) {
  SmacqGraph * ap;

  for (ap = this; ap->next_graph; ap=ap->nextGraph()) ;

  ap->next_graph = b;
}

/// Recursively initalize nodes in graph.
inline void SmacqGraph::init_all(DTS * dts, SmacqScheduler * sched, bool do_optimize) {
  for (SmacqGraph * g = this; g; g=g->nextGraph()) {

    // Insert a blank node before head so that it can use insert()
    if (g->argc) {
	// Actually copy ourselves, insert copy after us, make us stub
    	SmacqGraph * newg = new SmacqGraph(g->argc, g->argv);
	SmacqGraph_ptr save1 = newg; /// Try without
	SmacqGraph_ptr save2 = g; /// Try without
    	newg->next_graph = NULL;
	//fprintf(stderr, "Inserting %p after %p\n", newg, this);

	// Move my children to copy
	g->replace(newg);

	// Make new node our child
    	g->add_child(newg);

	// Make us the stub
    	g->argc = 0;
    }

    g->init_node_recursively(dts, sched);
  }

  this->print(stderr, 15);

  if (do_optimize) optimize();
}

inline void SmacqGraph::init_node(DTS * dts, SmacqScheduler * sched) {
  scheduler = sched;

  struct SmacqModule::smacq_init context;

  if (argc) {
  	context.islast = !(children[0].size());

	// Figure out if we have (non-stub) parent(s)
  	context.isfirst = true;
	for (std::vector<SmacqGraph*>::iterator i = parent.begin(); i != parent.end(); ++i) {
		if ((*i)->argc) {
			context.isfirst = false;
			break;
		}
	}
  	context.dts = dts;
  	context.self = this;
	context.scheduler = sched;

  	this->SmacqGraphNode::init(context);
  }
}

inline void SmacqGraph::init_node_recursively(DTS * dts, SmacqScheduler * sched) {
  init_node(dts, sched);

  FOREACH_CHILD(this, child->init_node_recursively(dts, sched));
}
  

inline SmacqGraph::SmacqGraph(int argc, char ** argv) 
  : next_graph(NULL), scheduler(NULL), children(1), numparents(0), refcount(0)
{
  SmacqGraphNode::set(argc, argv);
}

inline SmacqGraph * SmacqGraph::new_child(int argc, char ** argv) {
  SmacqGraph * newo = new SmacqGraph(argc, argv);
  add_child(newo);
  return newo;
}

/// Recursively clone a graph and all of it's children.  
/// Make the new graph be a child of the specified parent (which may be NULL).
inline SmacqGraph * SmacqGraph::clone(SmacqGraph * newParent) {
  SmacqGraph * newg;
  
  if (newParent) {
    newg = newParent->new_child(argc, argv);
  } else {
    newg = new SmacqGraph(argc, argv);
  }
  
  FOREACH_CHILD(this, child->clone(newg));

  // Clone next_graph(s) too
  if (next_graph) {
	  newg->next_graph = next_graph->clone(newParent);
  }

  return newg;
}

/// Remove the specified graph from the list of this graph's parents
inline void SmacqGraph::remove_parent(SmacqGraph * parent) {
  int i;
  if (!parent) return;

  assert(this->numparents);

  for (i = 0; i < this->numparents; i++) {
    if (this->parent[i] == parent) {
      this->numparents--;

      // Order doesn't matter; swap with last element.
      if (i < this->numparents) {
	this->parent[i] = this->parent[this->numparents];
      }

      // Erase old reference
      this->parent[this->numparents] = NULL;

      break;
    }
  }
}

inline void SmacqGraph::replace_child(SmacqGraph * oldchild, 
				      SmacqGraph * newchild) 
{

  FOREACH_CHILD(this, {
      if (child == oldchild) {
	replace_child(i, j, newchild);
	return;
      }
    });
}

inline void SmacqGraph::replace_child(int i, int j, SmacqGraph * newchild) {
  if (this->children[i][j] == newchild) return;

  //fprintf(stderr, "About to replace %p (child %d of %p) with %p\n", this->child[num], num, this, newchild);
  children[i][j]->remove_parent(this);
  newchild->add_parent(this);
  children[i][j] = newchild;
}

inline void SmacqGraph::remove_child(int i, int j) {
  //assert(!algebra.demux && !algebra.vector); //okay when called from shutdown
  int max = children[i].size() - 1;

  children[i][j]->remove_parent(this);
  children[i][j] = children[i][max];
  children[i].resize(max);
}

inline void SmacqGraph::remove_child(SmacqGraph * oldchild) {
  FOREACH_CHILD(this, {
      if (child == oldchild) {
	remove_child(i,j);
	return;
      }
    });
 }

// Remove all children.
inline void SmacqGraph::remove_children() {
  // Reference counting will cause orphaned children to shutdown 
  // automatically (after any more scheduled consumptions).
  while (children.size()) {
        // Work from last element up
        unsigned int el = children.size() - 1;
  
        // Remove all children of element "el"
        while (children[el].size()) {
                unsigned int el2 = children[el].size() - 1;
                remove_child(el,el2);
        }
        children.pop_back();
  }
  children.resize(1); // Size is always supposed to be >= 1.
}


inline double SmacqGraph::count_nodes() {
  double count = 1;

  FOREACH_CHILD(this, count+= child->count_nodes());

  if (numparents) {
    return count / numparents;
  }
  return count;
}

inline int SmacqGraph::print_one(FILE * fh, int indent) {
  int count = 1;

  fprintf(fh, "%p: ", this);

  for (int i=0; i<this->argc; i++) {
    fprintf(fh, " %s", this->argv[i]);
  }
  fprintf(fh, "\n");

  FOREACH_CHILD(this, {
      fprintf(fh, "%*s+ Child %d,%d is ", indent, "", i, j);
      count += child->print_one(fh, indent+2);
    });

  return count;
}

inline int SmacqGraph::print(FILE * fh, int indent) {
  double count = 0;
  SmacqGraph * ap;
  for (ap = this; ap; ap=ap->nextGraph()) {
    ap->print_one(fh, indent);
    count += ap->count_nodes();
  }
  fprintf(fh, "Total number of nodes = %g\n", count);
  return (int)count;
}

inline void SmacqGraph::add_parent(SmacqGraph * parent) {
  this->parent[this->numparents++] = parent;
}		

inline void SmacqGraph::share_children_of(SmacqGraph * g) {
  assert(g->children.size() == 1);
  if (!g->children[0].size()) return;

  for (unsigned int i = 0; i < g->children[0].size(); i++) {
    join(g->children[0][i].get());
  }
}

inline void SmacqGraph::dynamic_insert(SmacqGraph * g, DTS * dts) {
  // Tell parents to use new graph instead of me
  assert(numparents > 0);

  // replace_child could terminate me, so keep a ref
  SmacqGraph_ptr me = this;

  // Tell my parents to orphan me (will also tell my children 
  // about their new parents)
  while (numparents) {
    parent[0]->replace_child(this, g);
  }

  // And make me a child of the new graph
  g->join(this);

  // Go ahead and init 
  g->init_node(dts, this->scheduler);
}

/// Modify parent(s) and children to replace myself with the specified graph.
inline void SmacqGraph::replace(SmacqGraph * g) {
  if (children[0].size()) {
    assert(children.size() == 1);

    // Add children to join set
    SmacqGraph * down = NULL;
    for (unsigned int i = 0; i < children[0].size(); i++) {
      children[0][0]->remove_parent(this);
      if (down) { 
	down->add_graph(children[0][0].get());
      } else {
        down = children[0][0].get();
      }
    }

    // Join new graph with my children
    g->join(down);

    // Now safe to remove old references to children
    children[0].clear();
  }

  while (numparents) {
    parent[0]->replace_child(this, g);
  }
}

inline SmacqGraph * SmacqGraph::getChildInvariants(DTS* dts, SmacqScheduler* sched, DtsField& field) {
	if (children.size() == 1 && children[0].size() == 1) {
		return children[0][0]->getInvariants(dts, sched, field);
      	} else {
		return NULL;
 	}
}

inline SmacqGraph * SmacqGraph::getInvariants(DTS * dts, SmacqScheduler * sched, DtsField & field) {
  if (!algebra.stateless) {
    //fprintf(stderr, "%s is not stateless, so stopping invariant search\n", argv[0]);
    return NULL;
  }

  SmacqGraph * more = getChildInvariants(dts, sched, field);

  if (!instance) 
    init_node(dts, sched);

  if (!instance->usesOtherFields(field)) {
    SmacqGraph * result = new SmacqGraph(argc, argv);
    if (more) result->add_child(more);
    return result;
  } else {
    //fprintf(stderr, "%s uses other field, can't optimize\n", argv[0]);
    return more;
  }
}

inline bool SmacqGraph::live_children() {
  FOREACH_CHILD(this, {
      if (child && !child->shutdown)
	return true;
    });

  return false;
}

inline bool SmacqGraph::live_parents() {
  for (int i = 0; i < numparents; i++) {
    if (!parent[i]->shutdown) return true;
  }
  return false;
}

/// Decrement the reference count.
/// If the refcount is 0, then clean-up references and destroy 
inline void intrusive_ptr_release(SmacqGraph *o) { 
	o->refcount--;

	if (! o->refcount) {
		fprintf(stderr, "Auto shutdown %p\n", o);
  		if (o->scheduler) {
			// do_shutdown will remove child and parent 
			// references to us.
			o->scheduler->do_shutdown(o);
			
			// XXX.  These can be removed
			assert(o->children.size() <= 1);
	 		if (o->children.size()) 
				assert(o->children[0].size() == 0);
			assert(!o->numparents);
  		} else {
			o->remove_children();
		}

  		//o->next_graph = NULL;
		//delete o;  next_graph may still reference us
	}
}
#endif

