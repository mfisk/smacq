#ifndef SMACQ_GRAPH_H
#define SMACQ_GRAPH_H
#include <SmacqGraph-interface.h>
#include <IterativeScheduler-interface.h>

inline SmacqGraph::~SmacqGraph() {
  do_shutdown(this);
  //children.clear();
  //parent.clear();
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
inline void SmacqGraphContainer::list_tails(std::set<SmacqGraph *> &list) {
  for (unsigned int i = 0; i < head.size(); i++) {
	head[i]->list_tails(list);
  }
}

/// Recursively initialize a list of all the tails in this given graph
inline void SmacqGraph::list_tails(std::set<SmacqGraph *> &list) {
  bool has_child = false;

  FOREACH_CHILD(this, {
	has_child = true;
	child->list_tails(list);
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
	(*i)->add_child(newg);
  }
}

inline void SmacqGraphContainer::join(SmacqGraphContainer * newg, bool dofree) {
  if (!newg) return;

  if (head.empty()) {
	(*this) = *newg;
  } else {
  	std::set<SmacqGraph *> list;
  	std::set<SmacqGraph *>::iterator i;
  	list_tails(list);

  	for(i = list.begin(); i != list.end(); ++i) {
    	    for (unsigned int j = 0; j < newg->head.size(); j++) {
		(*i)->add_child(newg->head[j].get());
    	    }
  	}
  }

  if (dofree) delete newg;
  
}

inline void SmacqGraphContainer::join(SmacqGraph * newg) {
  if (!newg) { return; }
  if (head.empty()) {
	add_graph(newg);
	return;
  }

  std::set<SmacqGraph *> list;
  std::set<SmacqGraph *>::iterator i;
  list_tails(list);

  for(i = list.begin(); i != list.end(); ++i) {
	(*i)->add_child(newg);
  }
}

inline void SmacqGraphContainer::add_graph(SmacqGraph * b) {
  head.push_back(b);
}

inline void SmacqGraphContainer::add_graph(SmacqGraphContainer * b, bool dofree) {
  for (unsigned int i = 0; i < b->head.size(); i++) {
  	head.push_back(b->head[i]);
  }
  if (dofree) delete b;
}

/// Recursively initalize nodes in graph.
inline void SmacqGraphContainer::init(DTS * dts, SmacqScheduler * sched, bool do_optimize) {
  for (unsigned int i = 0; i < head.size(); i++) {
	head[i] = head[i]->init(dts, sched);
  }

  optimize();
}

/// Shutdown graphs
inline void SmacqGraphContainer::shutdown() {
  for (unsigned int i = 0; i < head.size(); i++) {
	SmacqGraph::do_shutdown(head[i].get());
	head.erase(i);
  }
}

/// Init graph and return replacement graph pointer
inline SmacqGraph * SmacqGraph::init(DTS * dts, SmacqScheduler * sched) {
  SmacqGraph * ret = this;

  // Insert a blank node before head so that it can use insert()
  if (argc) {
	// Actually copy ourselves, insert copy after us, make us stub
    	ret = new SmacqGraph(0, NULL);
	ret->add_child(this);
  }

  init_node_recursively(dts, sched);

  return ret;
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
  : scheduler(NULL), children(1), refcount(0)
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
inline SmacqGraphContainer * SmacqGraphContainer::clone(SmacqGraph * newParent) {
  SmacqGraphContainer * newg = new SmacqGraphContainer;

  for (unsigned int i = 0; i < head.size(); i++) {
	newg->head.push_back(head[i]->clone(newParent));
  }

  return newg;
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

  return newg;
}

/// Remove the specified graph from the list of this graph's parents
inline void SmacqGraph::remove_parent(SmacqGraph * p) {
  if (!p) return;

  assert(this->parent.size());

  for (unsigned i = 0; i < this->parent.size(); i++) {
    if (this->parent[i] == p) {
      this->parent.erase(i);
      return;
    }
  }
}

inline void SmacqGraph::replace_child(SmacqGraph * oldchild, 
				      SmacqGraphContainer * newchild) 
{

  FOREACH_CHILD(this, {
      if (child == oldchild) {
	replace_child(i, j, newchild);
	return;
      }
    });
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

inline void SmacqGraph::replace_child(int i, int j, SmacqGraphContainer * newchild) {
  // Remove old child
  remove_child(i,j);

  // Add new child(ren)
  for (unsigned int h = 0; h < newchild->head.size(); h++) {
  	add_child(newchild->head[h].get(), i);
  }
}

inline void SmacqGraph::replace_child(int i, int j, SmacqGraph * newchild) {
  // Remove old child
  remove_child(i,j);

  // Add new child(ren)
  add_child(newchild, i);
}

inline void SmacqGraph::remove_child(int i, int j) {
  //assert(!algebra.demux && !algebra.vector); //okay when called from shutdown
  children[i][j]->remove_parent(this);
  children[i].erase(j);
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


inline void SmacqGraph::print(FILE * fh, int indent) {
  fprintf(fh, "%p: ", this);

  for (int i=0; i<this->argc; i++) {
    fprintf(fh, " %s", this->argv[i]);
  }
  fprintf(fh, "\n");

  FOREACH_CHILD(this, {
      fprintf(fh, "%*s+ Child %d,%d is ", indent, "", i, j);
      child->print(fh, indent+2);
    });
}

inline void SmacqGraphContainer::print(FILE * fh, int indent) {
  for (unsigned int i = 0; i < head.size(); i++) {
    head[i]->print(fh, indent);
  }
}

inline void SmacqGraph::add_parent(SmacqGraph * p) {
  parent.push_back(p);
}		

inline void SmacqGraphContainer::share_children_of(SmacqGraph * g) {
  assert(g->children.size() == 1);
  if (!g->children[0].size()) return;

  for (unsigned int i = 0; i < g->children[0].size(); i++) {
    join(g->children[0][i].get());
  }
}

inline void SmacqGraph::dynamic_insert(SmacqGraph * g, DTS * dts) {
  // Tell parents to use new graph instead of me
  assert(parent.size() > 0);

  // replace_child could terminate me, so keep a ref
  refcount++;

  // Tell my parents to orphan me (will also tell my children 
  // about their new parents)
  while (parent.size()) {
    	parent[0]->replace_child(this, g);
  }

  // Make me a child of the new graph
  g->join(this);

  // safe to decrement refcount now
  refcount--;

  // Go ahead and init 
  g->init(dts, this->scheduler);
}

/// Modify parent(s) and children to replace myself with the specified graph.
inline void SmacqGraph::replace(SmacqGraphContainer * g) {
  if (children[0].size()) {
    SmacqGraphContainer down;

    // Get list of tails
    std::set<SmacqGraph*> list;
    std::set<SmacqGraph*>::iterator t;
    g->list_tails(list);

    // Join new graph with my children
    FOREACH_CHILD(this, 
    	for (t = list.begin(); t != list.end(); ++t) {
		(*t)->add_child(child, i);
    	}
	);

    // Now safe to remove me as old parent
    remove_children();
  }

  // Update parents
  while (parent.size()) {
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

inline SmacqGraph * SmacqGraphContainer::getInvariants(DTS * dts, SmacqScheduler * sched, DtsField & field) {
  //XXX haven't implemented invariants across parallel graphs
  if (head.size() == 1) {
	return head[0]->getInvariants(dts, sched, field);
  }
  return NULL;
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
  for (unsigned int i = 0; i < parent.size(); i++) {
    if (!parent[i]->shutdown) return true;
  }
  return false;
}

/// The caller should not refer to instance after this method is called.
/// The call may result in the object being destroyed.
inline void SmacqGraph::do_shutdown(SmacqGraph * f) {
  if (f->shutdown) {
    // Already shutdown, so do nothing
    // (This will happen when our children are shutdown).
    fprintf(stderr, "do_shutdown(%p) already done\n", f);
    return;
  }

  fprintf(stderr, "do_shutdown(%p)\n", f);

  f->shutdown = true;
  delete f->instance; // Call the destructor, which may callback to enqueue()
  f->instance = NULL;

  // Remove all children.
  f->remove_children();

  // Propagate to parents
  while (f->parent.size()) {
    // Work from end of list up.
    int i = f->parent.size() - 1; 

    // Parents will still have references (e.g. scheduler queues), so
    // reference counting won't shutdown our parents.
    // So, we act like a SIGPIPE and shutdown useless parents right
    // away.
    if (!f->parent[i]->shutdown && !f->parent[i]->live_children()) {
      // No reason to live if all former children gone!

      // Callee will remove parent from our parent list.
      do_shutdown(f->parent[i]);
    }
  }

  fprintf(stderr, "do_shutdown(%p) done\n", f);
}

/// Decrement the reference count.
/// If the refcount is 0, then clean-up references and destroy 
inline void intrusive_ptr_release(SmacqGraph *o) { 
	if (! --o->refcount) {
		//fprintf(stderr, "Auto shutdown %p\n", o);
		SmacqGraph::do_shutdown(o);

		// do_shutdown() may result in more references, so
		// destroy only if refcount still 0
		if (o->refcount == 0) {
  			delete o;
		}
	}
}
#endif

