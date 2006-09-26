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

METHOD SmacqGraph::~SmacqGraph() {
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
METHOD void SmacqGraph::add_child(SmacqGraph_ptr newo, unsigned int channel) {
  RecursiveLock l(children);

  assert(newo != this);
  if (channel >= children.size()) {
    children.resize(channel+1);
  }
  children[channel].push_back(newo);
  newo->add_parent(this);
}


/// Establish a parent/child relationship on the specified channel
METHOD void SmacqGraph::add_children(SmacqGraphContainer * newo, unsigned int channel) {
  using namespace boost::lambda;
  using namespace std;

  newo->head.foreach( bind(&SmacqGraph::add_child, this, _1, channel) );
}

/// Recursively initialize a list of all the tails of this bag of graphs
METHOD void SmacqGraphContainer::list_tails(std::set<SmacqGraph *> &list) {
  using namespace boost::lambda;
  using namespace std;

  head.foreach( bind(&SmacqGraph::list_tails, DEREF(_1), var(list)) );
}

/// Recursively initialize a list of all the tails in this given graph
METHOD void SmacqGraph::list_tails(std::set<SmacqGraph *> &list) {
  bool has_child = false;

  FOREACH_CHILD(this, {
	has_child = true;
	child->list_tails(list);
	});
  
  if (!has_child) {
    list.insert(this); // Will ignore dups
  }
}

METHOD void SmacqGraph::join(SmacqGraph * newg) {
  if (!newg) { return; }

  std::set<SmacqGraph *> list;
  std::set<SmacqGraph *>::iterator i;
  list_tails(list);

  for(i = list.begin(); i != list.end(); ++i) {
	(*i)->add_child(newg);
  }
}

METHOD void SmacqGraph::join(SmacqGraphContainer * newg, bool dofree) {
  if (!newg) { return; }

  std::set<SmacqGraph *> list;
  std::set<SmacqGraph *>::iterator i;
  list_tails(list);

  for(i = list.begin(); i != list.end(); ++i) {
	(*i)->add_children(newg);
  }

  if (dofree) {
	delete newg;
  }
}

METHOD void SmacqGraphContainer::join(SmacqGraphContainer * newg, bool dofree) {
  if (!newg) return;

  if (head.empty()) {
	(*this) = *newg;
  } else {
  	std::set<SmacqGraph *> list;
  	std::set<SmacqGraph *>::iterator i;
  	list_tails(list);

  	for(i = list.begin(); i != list.end(); ++i) {
	    (*i)->add_children(newg);
  	}
  }

  if (dofree) {
	delete newg;
  }
}

METHOD void SmacqGraphContainer::join(SmacqGraph * newg) {
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

METHOD void SmacqGraphContainer::add_graph(SmacqGraph * b) {
  if (b) head.insert(b);
}

METHOD void SmacqGraphContainer::add_graph(SmacqGraphContainer * b, bool dofree) {
  using namespace boost::lambda;
  using namespace std;

  if (!b) return;

  b->head.foreach( bind(&ThreadSafeMultiSet<SmacqGraph_ptr>::insert, &head, _1) );
  if (dofree) delete b;
}

/// Recursively initalize nodes in graph.
METHOD void SmacqGraphContainer::init(DTS * dts, SmacqScheduler * sched, bool do_optimize) {
  using namespace boost::lambda;
  using namespace std;

  head.foreach( _1 = bind(&SmacqGraph::init, DEREF(_1), dts, sched) );

  optimize();
}

/// Shutdown graphs
METHOD void SmacqGraphContainer::shutdown() {
  using namespace boost::lambda;
  using namespace std;

  head.foreach( bind(&SmacqGraph::do_shutdown, _1) );
  head.clear();
}

/// Init graph and return replacement graph pointer
METHOD SmacqGraph * SmacqGraph::init(DTS * dts, SmacqScheduler * sched) {
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

#include <boost/lambda/if.hpp>

METHOD void SmacqGraph::init_node(DTS * dts, SmacqScheduler * sched) {
  using namespace boost::lambda;
  using namespace std;
  RecursiveLock l (this);

  scheduler = sched;

  struct SmacqModule::smacq_init context;

  if (argc) {
  	context.islast = !(children[0].size());

	// Figure out if we have (non-stub) parent(s)
  	context.isfirst = true;

	if (parent.has_if( bind(&SmacqGraph::argc, _1) )) {
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

METHOD void SmacqGraph::init_node_recursively(DTS * dts, SmacqScheduler * sched) {
  init_node(dts, sched);

  FOREACH_CHILD(this, child->init_node_recursively(dts, sched));
}
  

METHOD SmacqGraph::SmacqGraph(int argc, char ** argv) 
		  : instance(NULL), q(RINGSIZE), scheduler(NULL)
{
  set(argc, argv);
}

METHOD SmacqGraph * SmacqGraph::new_child(int argc, char ** argv) {
  SmacqGraph * newo = new SmacqGraph(argc, argv);
  add_child(newo);
  return newo;
}


METHOD void SmacqGraphContainer::add_clone(SmacqGraph_ptr x, SmacqGraph * newParent) {
   this->head.insert(x->clone(newParent));
}

/// Recursively clone a graph and all of it's children.  
/// Make the new graph be a child of the specified parent (which may be NULL).
METHOD SmacqGraphContainer * SmacqGraphContainer::clone(SmacqGraph * newParent) {
  using namespace boost::lambda;
  using namespace std;

  SmacqGraphContainer * newg = new SmacqGraphContainer;

  head.foreach( bind(&SmacqGraphContainer::add_clone, newg, _1, newParent));

  return newg;
}

/// Recursively clone a graph and all of it's children.  
/// Make the new graph be a child of the specified parent (which may be NULL).
METHOD SmacqGraph * SmacqGraph::clone(SmacqGraph * newParent) {
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
METHOD void SmacqGraph::remove_parent(SmacqGraph * p) {
  if (!p) return;

  assert(this->parent.size());

  parent.erase(p);
}

METHOD void SmacqGraph::replace_child(SmacqGraph * oldchild, 
				      SmacqGraphContainer * newchild) 
{
  FOREACH_CHILD(this, {
      if (child == oldchild) {
	replace_child(i, j, newchild);
	return;
      }
    });
}

METHOD void SmacqGraph::replace_child(SmacqGraph * oldchild, 
				      SmacqGraph * newchild) 
{
  FOREACH_CHILD(this, {
      if (child == oldchild) {
	replace_child(i, j, newchild);
	return;
      }
    });
}

METHOD void SmacqGraph::replace_child(int i, int j, SmacqGraphContainer * newchild) {
  // Remove old child
  remove_child_bynum(i,j);

  // Add new child(ren)
  for (unsigned int h = 0; h < newchild->head.size(); h++) {
  	add_child(newchild->head[h].get(), i);
  }
}

METHOD void SmacqGraph::replace_child(int i, int j, SmacqGraph * newchild) {
  // Remove old child
  remove_child_bynum(i,j);

  // Add new child(ren)
  add_child(newchild, i);
}

METHOD void SmacqGraph::remove_child_bynum(int i, int j) {
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

METHOD void SmacqGraph::remove_child(SmacqGraph_ptr oldchild) {
  FOREACH_CHILD(this, {
      if (child == oldchild) {
	remove_child_bynum(i,j);
	return;
      }
    });
}

// Remove all children.
METHOD void SmacqGraph::remove_children() {
  RecursiveLock l(children);

  // Reference counting will cause orphaned children to shutdown 
  // automatically (after any more scheduled consumptions).
  //children.foreach( bind(&SmacqGraph::remove_child, this, DEREF(_1)) );
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


METHOD void SmacqGraph::print(FILE * fh, int indent) {
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

METHOD void SmacqGraphContainer::print(FILE * fh, int indent) {
  for (unsigned int i = 0; i < head.size(); i++) {
    head[i]->print(fh, indent);
  }
}

METHOD std::string SmacqGraph::print_query_tail() {
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

METHOD std::string SmacqGraph::print_query() {
  std::string s;
  std::set<SmacqGraph*> list;
  std::set<SmacqGraph*>::iterator i;
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

/// This function is IDENTICAL to SmacqGraph::print_query().
/// So much for polymorphism
METHOD std::string SmacqGraphContainer::print_query() {
  std::string s;
  std::set<SmacqGraph*> list;
  std::set<SmacqGraph*>::iterator i;
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

METHOD void SmacqGraph::add_parent(SmacqGraph * p) {
  parent.insert(p);
}		

METHOD void SmacqGraphContainer::share_children_of(SmacqGraph * g) {
  RecursiveLock l(g->children);

  assert(g->children.size() == 1);
  if (!g->children[0].size()) return;

  for (unsigned int i = 0; i < g->children[0].size(); i++) {
    join(g->children[0][i].get());
  }
}

METHOD void SmacqGraph::dynamic_insert(SmacqGraph * g, DTS * dts) {
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
METHOD void SmacqGraph::replace(SmacqGraphContainer * g) {
  RecursiveLock l(this);

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

METHOD SmacqGraph * SmacqGraph::getChildInvariants(DTS* dts, SmacqScheduler* sched, DtsField& field) {
	RecursiveLock l(children);
	if (children.size() == 1 && children[0].size() == 1) {
		return children[0][0]->getInvariants(dts, sched, field);
      	} else {
		return NULL;
 	}
}

METHOD SmacqGraph * SmacqGraphContainer::getInvariants(DTS * dts, SmacqScheduler * sched, DtsField & field) {
  //XXX haven't implemented invariants across parallel graphs
  if (head.size() == 1) {
	return head[0]->getInvariants(dts, sched, field);
  }
  return NULL;
} 

METHOD SmacqGraph * SmacqGraph::getInvariants(DTS * dts, SmacqScheduler * sched, DtsField & field) {
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

METHOD bool SmacqGraph::live_children() {
  FOREACH_CHILD(this, {
      if (child && !child->shutdown.get())
	return true;
    });

  return false;
}

METHOD bool SmacqGraph::live_parents() {
  for (unsigned int i = 0; i < parent.size(); i++) {
    if (!parent[i]->shutdown.get()) return true;
  }
  return false;
}

/// The caller should not refer to instance after this method is called.
/// The call may result in the object being destroyed.
METHOD void SmacqGraph::do_shutdown(SmacqGraph_ptr f) {
  // Our argument is a SmacqGraph_ptr in order to keep our reference count up 
  // so that when we remove links to us, we don't end up calling our own 
  // destructor while we're still running.

  //// Don't take a RecursiveLock in this scope since it might also 
  //// try to unlock us after we're destroyed.
  // RecursiveLock l(f);

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
  const std::vector<SmacqGraph*> parents = f->parent.snapshot();

  std::vector<SmacqGraph*>::const_iterator i;
  for (i = parents.begin(); i != parents.end(); ++i) {
	// Keep a reference around while we're working on it
	SmacqGraph_ptr ip = (*i);

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
METHOD void intrusive_ptr_release(SmacqGraph *o) { 
	if (! o->refcount.decrement()) {
		//fprintf(stderr, "shutdown %p because refcount = 0\n", o);

		o->refcount.increment();
		SmacqGraph::do_shutdown(o);
		o->refcount.decrement();

		// do_shutdown() may result in more references, so
		// destroy only if refcount still 0
		if (o->refcount.get() == 0) {
  			delete o;
		}
	}
}

METHOD SmacqGraphContainer::SmacqGraphContainer(ThreadSafeMultiSet<SmacqGraph_ptr> & children) {
  for (unsigned int i = 0; i < children.size(); ++i) {
	add_graph(children[i].get());
  }
}

METHOD void SmacqGraph::seed_produce() {
   SGDEBUG("seed_produce()");

   mustProduce.set();
   scheduler->produceq.enqueue(this);
}  

METHOD void SmacqGraph::log(const char * format, ...) {
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
 
METHOD void SmacqGraph::runable(DtsObject d) {
  SGDEBUG("inputq got %p", d.get());
  bool change = inputq.enqueue(d);

  if (change && ! mustProduce.get()) {
	SGDEBUG("placed on consumeq");
        scheduler->consumeq.enqueue(this);
  } 
}   

/// We are done handling a mustProduce
METHOD void SmacqGraph::produce_done() {
  SGDEBUG("produce_done()");

  if (mustProduce.clear() && !inputq.empty()) {
      scheduler->consumeq.enqueue(this);
  }

}

#include <SmacqModule-interface.h>

/// Setup a graph node based on the given argument vector.
METHOD bool SmacqGraph::set(int argc, char ** argv) {
  this->argv = argv;
  this->argc = argc;
  
  if (argc && !this->load_module()) {
    exit(-1);
    return false;
  }

  return true;
}

/// Instantiate this node.
METHOD bool SmacqGraph::load_module() {
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

