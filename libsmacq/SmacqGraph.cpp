#ifndef SMACQ_GRAPH_CPP
#define SMACQ_GRAPH_CPP
#include <SmacqGraph.h>
#include <SmacqScheduler.h>
#include <string>
#define RINGSIZE 4

#define SGDEBUG(args...) if (scheduler && scheduler->isDebug()) { log(args); }

#ifndef METHOD
#define METHOD 
#endif

METHOD SmacqGraphNode::~SmacqGraphNode() {
  //log("shutdown from destructor (e.g. refcount==0");

  // Make sure that do_shutdown doesn't end up calling the destructor again
  refcount.increment();

  do_shutdown(this);
  refcount.decrement();

  //log("shutdown from destructor (e.g. refcount==0) done");
  //children.clear();
  //parent.clear();
}

/// Establish a parent/child relationship on the specified channel
METHOD void SmacqGraphNode::add_child(SmacqGraphNode_ptr newo, unsigned int channel) {
  RECURSIVE_LOCK(children);

  assert(newo != this);
  if (channel >= children.size()) {
    children.resize(channel+1);
  }
  children[channel].push_back(newo);
  newo->add_parent(this);
}


/// Establish a parent/child relationship on the specified channel
METHOD void SmacqGraphNode::add_children(SmacqGraph * newo, unsigned int channel) {
  using namespace boost::lambda;
  using namespace std;

  newo->head.foreach( bind(&SmacqGraphNode::add_child, this, _1, channel) );
}

/// Recursively initialize a list of all the tails of this bag of graphs
METHOD void SmacqGraph::list_tails(std::set<SmacqGraphNode *> &list) {
  using namespace boost::lambda;
  using namespace std;

  head.foreach( bind(&SmacqGraphNode::list_tails, DEREF(_1), var(list)) );
}

/// Recursively initialize a list of all the tails in this given graph
METHOD void SmacqGraphNode::list_tails(std::set<SmacqGraphNode *> &list) {
  bool has_child = false;

  FOREACH_CHILD(this, {
	has_child = true;
	child->list_tails(list);
	});
  
  if (!has_child) {
    list.insert(this); // Will ignore dups
  }
}

METHOD void SmacqGraphNode::join(SmacqGraphNode * newg) {
  if (!newg) { return; }

  std::set<SmacqGraphNode *> list;
  std::set<SmacqGraphNode *>::iterator i;
  list_tails(list);

  for(i = list.begin(); i != list.end(); ++i) {
	(*i)->add_child(newg);
  }
}

METHOD void SmacqGraphNode::join(SmacqGraph * newg, bool dofree) {
  if (!newg) { return; }

  std::set<SmacqGraphNode *> list;
  std::set<SmacqGraphNode *>::iterator i;
  list_tails(list);

  for(i = list.begin(); i != list.end(); ++i) {
	(*i)->add_children(newg);
  }

  if (dofree) {
	delete newg;
  }
}

METHOD void SmacqGraph::join(SmacqGraph * newg, bool dofree) {
  if (!newg) return;

  if (head.empty()) {
	(*this) = *newg;
  } else {
  	std::set<SmacqGraphNode *> list;
  	std::set<SmacqGraphNode *>::iterator i;
  	list_tails(list);

  	for(i = list.begin(); i != list.end(); ++i) {
	    (*i)->add_children(newg);
  	}
  }

  if (dofree) {
	delete newg;
  }
}

METHOD void SmacqGraph::join(SmacqGraphNode * newg) {
  if (!newg) { return; }
  if (head.empty()) {
	add_graph(newg);
	return;
  }

  std::set<SmacqGraphNode *> list;
  std::set<SmacqGraphNode *>::iterator i;
  list_tails(list);

  for(i = list.begin(); i != list.end(); ++i) {
	(*i)->add_child(newg);
  }
}

METHOD void SmacqGraph::add_graph(SmacqGraphNode * b) {
  if (b) head.insert(b);
}

METHOD void SmacqGraph::add_graph(SmacqGraph * b, bool dofree) {
  using namespace boost::lambda;
  using namespace std;

  if (!b) return;

  b->head.foreach( bind(&ThreadSafeMultiSet<SmacqGraphNode_ptr>::insert, &head, _1) );
  if (dofree) delete b;
}

/// Recursively initalize nodes in graph.
METHOD void SmacqGraph::init(DTS * dts, SmacqScheduler * sched, bool do_optimize) {
  using namespace boost::lambda;
  using namespace std;

  head.foreach( _1 = bind(&SmacqGraphNode::init, DEREF(_1), dts, sched) );

  if (do_optimize) optimize();
}

/// Shutdown graphs
METHOD void SmacqGraph::shutdown() {
  using namespace boost::lambda;
  using namespace std;

  head.foreach( bind(&SmacqGraphNode::do_shutdown, _1) );
  head.clear();
}

/// Init graph and return replacement graph pointer
METHOD SmacqGraphNode * SmacqGraphNode::init(DTS * dts, SmacqScheduler * sched) {
  SmacqGraphNode * ret = this;

  // Insert a blank node before head so that it can use insert()
  if (argc) {
	// Actually copy ourselves, insert copy after us, make us stub
    	ret = new SmacqGraphNode(0, NULL);
	ret->add_child(this);
  }

  init_node_recursively(dts, sched);

  return ret;
}

#include <boost/lambda/if.hpp>

METHOD void SmacqGraphNode::init_node(DTS * dts, SmacqScheduler * sched) {
  using namespace boost::lambda;
  using namespace std;
  RECURSIVE_LOCK (this);

  scheduler = sched;

  struct SmacqModule::smacq_init context;

  if (argc) {
  	context.islast = !(children[0].size());

	// Figure out if we have (non-stub) parent(s)
  	context.isfirst = true;

	if (parent.has_if( bind(&SmacqGraphNode::argc, _1) )) {
		context.isfirst = false;
	}

  	context.dts = dts;
  	context.self = this;
	context.scheduler = sched;

        if (!instance) {
  		assert(argc && constructor);

  		/* fprintf(stderr, "init_modules on graph node %p %s\n", f, f->argv[0]); */

  		context.argc = argc;
  		context.argv = argv;

  		assert(constructor);
  		instance = (SmacqModule*)1; // This will prevent us from recursively initing ourselves
  		instance = constructor(&context);

  		if (! instance) {
     			fprintf(stderr, "Error initializing module %s\n", argv[0]);	 	
			exit(-1);
  		}
	}
  }
}

METHOD void SmacqGraphNode::init_node_recursively(DTS * dts, SmacqScheduler * sched) {
  init_node(dts, sched);

  FOREACH_CHILD(this, child->init_node_recursively(dts, sched));
}
  

METHOD SmacqGraphNode::SmacqGraphNode(int argc, const char ** argv) 
		  : instance(NULL), q(RINGSIZE), scheduler(NULL)
{
  memset(&algebra, 0, sizeof(algebra));
  set(argc, argv);
}

METHOD SmacqGraphNode * SmacqGraphNode::new_child(int argc, const char ** argv) {
  SmacqGraphNode * newo = new SmacqGraphNode(argc, argv);
  add_child(newo);
  return newo;
}


METHOD void SmacqGraph::add_clone(SmacqGraphNode_ptr x, SmacqGraphNode * newParent) {
   this->head.insert(x->clone(newParent));
}

/// Recursively clone a graph and all of it's children.  
/// Make the new graph be a child of the specified parent (which may be NULL).
METHOD SmacqGraph * SmacqGraph::clone(SmacqGraphNode * newParent) {
  using namespace boost::lambda;
  using namespace std;

  SmacqGraph * newg = new SmacqGraph;

  head.foreach( bind(&SmacqGraph::add_clone, newg, _1, newParent));

  return newg;
}

/// Recursively clone a graph and all of it's children.  
/// Make the new graph be a child of the specified parent (which may be NULL).
METHOD SmacqGraphNode * SmacqGraphNode::clone(SmacqGraphNode * newParent) {
  SmacqGraphNode * newg;
  
  if (newParent) {
    newg = newParent->new_child(argc, argv);
  } else {
    newg = new SmacqGraphNode(argc, argv);
  }
 
  FOREACH_CHILD(this, child->clone(newg));

  return newg;
}

/// Remove the specified graph from the list of this graph's parents
METHOD void SmacqGraphNode::remove_parent(SmacqGraphNode * p) {
  if (!p) return;

  assert(this->parent.size());

  parent.erase(p);
}

METHOD void SmacqGraphNode::replace_child(SmacqGraphNode * oldchild, 
				      SmacqGraph * newchild) 
{
  FOREACH_CHILD(this, {
      if (child == oldchild) {
	replace_child(i, j, newchild);
	return;
      }
    });
}

METHOD void SmacqGraphNode::replace_child(SmacqGraphNode * oldchild, 
				      SmacqGraphNode * newchild) 
{
  FOREACH_CHILD(this, {
      if (child == oldchild) {
	replace_child(i, j, newchild);
	return;
      }
    });
}

METHOD void SmacqGraphNode::replace_child(int i, int j, SmacqGraph * newchild) {
  // Remove old child
  remove_child_bynum(i,j);

  // Add new child(ren)
  for (unsigned int h = 0; h < newchild->head.size(); h++) {
  	add_child(newchild->head[h].get(), i);
  }
}

METHOD void SmacqGraphNode::replace_child(int i, int j, SmacqGraphNode * newchild) {
  // Remove old child
  remove_child_bynum(i,j);

  // Add new child(ren)
  add_child(newchild, i);
}

METHOD void SmacqGraphNode::remove_child_bynum(int i, int j) {
  children[i][j]->remove_parent(this);

  /*
  // Debugging output
  fprintf(stderr, "child %p losing parent %p, whose refcount is down to %d\n", children[i][j].get(), this, children[i][j]->refcount);
  if (children[i][j]->parent.size()) {
	fprintf(stderr, "child %p still has parent %p\n", children[i][j].get(), children[i][j]->parent[0]);
  }
  */
  
  children[i].erase(j);
}

METHOD void SmacqGraphNode::remove_child(SmacqGraphNode_ptr oldchild) {
  FOREACH_CHILD(this, {
      if (child == oldchild) {
	remove_child_bynum(i,j);
	return;
      }
    });
}

// Remove all children.
METHOD void SmacqGraphNode::remove_children() {
  RECURSIVE_LOCK(children);

  // Reference counting will cause orphaned children to shutdown 
  // automatically (after any more scheduled consumptions).
  //children.foreach( bind(&SmacqGraphNode::remove_child, this, DEREF(_1)) );
  while (children.size()) {
        // Remove all children of last element
	unsigned int i = children.size() - 1;
        while (children[i].size()) {
		SGDEBUG("remove_children %p", children[i][0].get());
                remove_child_bynum(i,0);
        }
        children.pop_back();
  }
  children.resize(1); // Size is always supposed to be >= 1.  Empty -> 1
}


METHOD void SmacqGraphNode::print(FILE * fh, int indent) {
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

METHOD void SmacqGraph::print(FILE * fh, int indent) {
  for (unsigned int i = 0; i < head.size(); i++) {
    head[i]->print(fh, indent);
  }
}

METHOD std::string SmacqGraphNode::print_query_tail() {
  std::string s;
  if (argc > 0) {
	s += argv[0];
	s += " ";
  }

  for (int i = 1; i < argc; ++i) {
	s += "'";
	s += argv[i];
	s += "' ";
  }

  // Recurse upwards
  if (parent.size()) {
	s += "from (";
  	for (unsigned int i = 0; i < parent.size(); ++i) {
		if (! parent[i]->argc) {
			break; // Stub
		}
		if (i > 0) {
			s += " + ";
		}
		s += parent[i]->print_query_tail();
	}
	s += ")";
  }

  return s;
}

METHOD std::string SmacqGraphNode::print_query() {
  std::string s;
  std::set<SmacqGraphNode*> list;
  std::set<SmacqGraphNode*>::iterator i;
  list_tails(list);
  bool first = true;

  for(i = list.begin(); i != list.end(); ++i) {
	if (!first) {
	  s += " + ";
	} else {
	  first = false;
        }
		
	s += (*i)->print_query_tail();
  }
  return s;
}

/// This function is IDENTICAL to SmacqGraphNode::print_query().
/// So much for polymorphism
METHOD std::string SmacqGraph::print_query() {
  std::string s;
  std::set<SmacqGraphNode*> list;
  std::set<SmacqGraphNode*>::iterator i;
  list_tails(list);
  bool first = true;

  for(i = list.begin(); i != list.end(); ++i) {
	if (!first) {
	  s += " + ";
	} else {
	  first = false;
        }
		
	s += (*i)->print_query_tail();
  }
  return s;
}

METHOD void SmacqGraphNode::add_parent(SmacqGraphNode * p) {
  parent.insert(p);
}		

METHOD void SmacqGraph::share_children_of(SmacqGraphNode * g) {
  RECURSIVE_LOCK(g->children);

  assert(g->children.size() == 1);
  if (!g->children[0].size()) return;

  for (unsigned int i = 0; i < g->children[0].size(); i++) {
    join(g->children[0][i].get());
  }
}

METHOD void SmacqGraphNode::dynamic_insert(SmacqGraphNode * g, DTS * dts) {
  // Tell parents to use new graph instead of me
  assert(parent.size() > 0);

  // replace_child could terminate me, so keep a ref
  refcount.increment();

  // Tell my parents to orphan me (will also tell my children 
  // about their new parents)
  while (parent.size()) {
    	parent[0]->replace_child(this, g);
  }

  // Make me a child of the new graph
  g->join(this);

  // safe to decrement refcount now
  refcount.decrement();

  // Go ahead and init 
  g->init(dts, this->scheduler);
}

/// Modify parent(s) and children to replace myself with the specified graph.
METHOD void SmacqGraphNode::replace(SmacqGraph * g) {
  RECURSIVE_LOCK(this);

  if (children[0].size()) {
    SmacqGraph down;

    // Get list of tails
    std::set<SmacqGraphNode*> list;
    std::set<SmacqGraphNode*>::iterator t;
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

METHOD SmacqGraphNode * SmacqGraphNode::getChildInvariants(DTS* dts, SmacqScheduler* sched, DtsField& field) {
	RECURSIVE_LOCK(children);
	if (children.size() == 1 && children[0].size() == 1) {
		return children[0][0]->getInvariants(dts, sched, field);
      	} else {
		return NULL;
 	}
}

METHOD SmacqGraphNode * SmacqGraph::getInvariants(DTS * dts, SmacqScheduler * sched, DtsField & field) {
  //XXX haven't implemented invariants across parallel graphs
  if (head.size() == 1) {
	return head[0]->getInvariants(dts, sched, field);
  }
  return NULL;
} 

METHOD SmacqGraphNode * SmacqGraphNode::getInvariants(DTS * dts, SmacqScheduler * sched, DtsField & field) {
  if (!algebra.stateless) {
    //fprintf(stderr, "%s is not stateless, so stopping invariant search\n", argv[0]);
    return NULL;
  }

  SmacqGraphNode * more = getChildInvariants(dts, sched, field);

  if (!instance) 
    init_node(dts, sched);

  if (!instance->usesOtherFields(field)) {
    SmacqGraphNode * result = new SmacqGraphNode(argc, argv);
    if (more) result->add_child(more);
    return result;
  } else {
    //fprintf(stderr, "%s uses other field, can't optimize\n", argv[0]);
    return more;
  }
}

METHOD bool SmacqGraphNode::live_children() {
  FOREACH_CHILD(this, {
      if (child && !child->shutdown.get())
	return true;
    });

  return false;
}

METHOD bool SmacqGraphNode::live_parents() {
  for (unsigned int i = 0; i < parent.size(); i++) {
    if (!parent[i]->shutdown.get()) return true;
  }
  return false;
}

/// The caller should not refer to instance after this method is called.
/// The call may result in the object being destroyed.
METHOD void SmacqGraphNode::do_shutdown(SmacqGraphNode_ptr f) {
  // Our argument is a SmacqGraphNode_ptr in order to keep our reference count up 
  // so that when we remove links to us, we don't end up calling our own 
  // destructor while we're still running.

  //// Don't take a RecursiveLock in this scope since it might also 
  //// try to unlock us after we're destroyed.
  // RECURSIVE_LOCK(f);

  // Atomically set shutdown.  Returns false if we set it, true if already set.
  if (!f->shutdown.set()) {
    // Already shutdown, so do nothing
    // (This will happen when our children are shutdown).
    //fprintf(stderr, "do_shutdown(%p) already done\n", f);
    return;
  }

  //f->log("do_shutdown");

  delete f->instance; // Call the destructor, which may callback to enqueue()
  f->instance = NULL;

  // Remove all children.
  f->remove_children();

  // Make an immutable copy
  const std::vector<SmacqGraphNode*> parents = f->parent.snapshot();

  std::vector<SmacqGraphNode*>::const_iterator i;
  for (i = parents.begin(); i != parents.end(); ++i) {
	// Keep a reference around while we're working on it
	SmacqGraphNode_ptr ip = (*i);

	//f->log("do_shutdown propagating to parent %p", ip.get());
	ip->remove_child(f);
	//f->log("do_shutdown detached parent %p", ip.get());

	if (!ip->live_children()) {
    		// Parents may still have references (e.g. scheduler queues), so
    		// reference counting won't necessarily shutdown our parents.
    		// So, we act like a SIGPIPE and shutdown useless parents right
    		// away.

		//f->log("do_shutdown propagating to parent %p", ip.get());
		do_shutdown((*i));
	} 
  }

  assert(f->parent.empty()); // Should be no links to parents left

  //f->log("do_shutdown done");
}

/// Decrement the reference count.
/// If the refcount is 0, then clean-up references and destroy 
METHOD void intrusive_ptr_release(SmacqGraphNode *o) { 
	if (! o->refcount.decrement()) {
		//fprintf(stderr, "shutdown %p because refcount = 0\n", o);

		o->refcount.increment();
		SmacqGraphNode::do_shutdown(o);
		o->refcount.decrement();

		// do_shutdown() may result in more references, so
		// destroy only if refcount still 0
		if (o->refcount.get() == 0) {
  			delete o;
		}
	}
}

METHOD SmacqGraph::SmacqGraph(ThreadSafeMultiSet<SmacqGraphNode_ptr> & children) {
  for (unsigned int i = 0; i < children.size(); ++i) {
	add_graph(children[i].get());
  }
}

METHOD void SmacqGraphNode::seed_produce() {
   SGDEBUG("seed_produce()");

   mustProduce.set();
   scheduler->produceq.enqueue(this);
}  

METHOD void SmacqGraphNode::log(const char * format, ...) {
  va_list args;
  char buf[101];
  snprintf(buf, 100, "%20s %10p %10p ", argv ? argv[0] : NULL, this, (void*)pthread_self());

  std::string output(buf);

  va_start( args, format );
  vsnprintf(buf, 100, format, args );
  va_end( args );

  output += buf;
  fprintf(stderr, "** %s\n", output.c_str());
}
 
METHOD void SmacqGraphNode::runable(DtsObject d) {
  SGDEBUG("inputq got %p", d.get());
  bool change = inputq.enqueue(d);

  if (change && ! mustProduce.get()) {
	SGDEBUG("placed on consumeq");
        scheduler->consumeq.enqueue(this);
  } 
}   

/// We are done handling a mustProduce
METHOD void SmacqGraphNode::produce_done() {
  SGDEBUG("produce_done()");

  if (mustProduce.clear() && !inputq.empty()) {
      scheduler->consumeq.enqueue(this);
  }

}

#include <SmacqModule-interface.h>

/// Setup a graph node based on the given argument vector.
METHOD bool SmacqGraphNode::set(int argc, const char ** argv) {
  this->argv = argv;
  this->argc = argc;
  
  if (argc && !this->load_module()) {
    exit(-1);
    return false;
  }

  return true;
}

/// Instantiate this node.
METHOD bool SmacqGraphNode::load_module() {
  struct smacq_functions * modtable;
  
  modtable = (struct smacq_functions*)smacq_find_module(&module, "SMACQ_HOME", "modules", "%s/smacq_%s", "smacq_%s_table", argv[0]);

  if (modtable) {
    constructor = modtable->constructor;
    algebra = modtable->algebra;

    return true;
  } else {
    constructor = NULL;
    
    fprintf(stderr, "Error: unable to find module %s (Need to set %s?)\n", argv[0], "SMACQ_HOME");

    return false;
  }
}

#endif

