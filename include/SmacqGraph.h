#ifndef LIBSMACQ_DATAFLOW_H
#define LIBSMACQ_DATAFLOW_H

#include <SmacqGraphNode.h>
#include <DynamicArray.h>
#include <set>
#include <vector>

#define FOREACH_CHILD(x, y)					    \
  for (unsigned int i = 0; i < (x)->children.size(); i ++)	    \
    for (unsigned int j = 0; j < (x)->children[i].size(); j ++) {   \
      SmacqGraph *& child = (x)->children[i][j];		    \
      y;							    \
    }

class DTS;
class SmacqGraph;

typedef void smacq_filter_callback_fn(char * operation, int argc, char ** argv, void * data);

/// This is pure-virtual class used to instantiate callback objects
/// for SmacqGraph::foreach_tail().
class SmacqGraphCallback {
 public:
  virtual void callback(SmacqGraph *g) = 0;
  virtual ~SmacqGraphCallback() {}
};

class joincallback : public SmacqGraphCallback {
 public:
  joincallback(SmacqGraph * g) : newg(g) {}
  void callback(SmacqGraph * g);

 private:
  SmacqGraph * newg;
  std::set<SmacqGraph*> seen;
};

/// A graph of SmacqGraphNode nodes. 
class SmacqGraph : private SmacqGraphNode {
  friend class IterativeScheduler;

 public:
  SmacqGraph(int argc, char ** argv);
  ~SmacqGraph();

  /// This method must be called before a graph is used.
  void init(DTS * dts, SmacqScheduler *);

  /// @name Parent/Child Relationships
  /// @{

  void replace(SmacqGraph *);

  void add_child(SmacqGraph * child, unsigned int channel = 0);
  void remove_parent(SmacqGraph * parent);
  void remove_child(int, int);
  void replace_child(int, int, SmacqGraph * newchild);
  void replace_child(SmacqGraph * oldchild, SmacqGraph * newchild);
  /// @}

  /// @name Combining Graphs
  /// A SmacqGraph can have multiple heads and tails and
  /// may even be disconnected. 
  /// @{
  
  /// Attach the specified graph onto the tail(s) of the graph.
  void join(SmacqGraph * g);

  /// Add a new graph head.
  void add_graph(SmacqGraph * b);

  /// Get the next graph head.
  SmacqGraph * nextGraph() const { return next_graph; }

  /// Return a multi-headed graph consisting of my children.
  SmacqGraph * children_as_heads();

  /// @}

  /// @name Factories
  /// @{

  /// Parse a query and construct a new graph to execute it 
  static SmacqGraph * newQuery(DTS * tenv, int argc, char ** argv);

  /// Construct a new graph using the given arguments.  The new graph
  /// is automatically attached as a child of the current graph.
  SmacqGraph * new_child(int argc, char ** argv);

  /// Recursively clone a graph.  The clone is made a child of
  /// newParent, unless newParent is NULL.
  SmacqGraph * clone(SmacqGraph * newParent);
  /// @}

  double count_nodes();
  int print(FILE * fh, int indent);
  void downstream_filters(smacq_filter_callback_fn callback, void * data);
  void optimize();

 private:
  void downstream_filter_one(smacq_filter_callback_fn callback, void * data);
  void init_node_one(DTS * dts, SmacqScheduler *);
  int print_one(FILE * fh, int indent);
  void add_parent(SmacqGraph * parent);
  void foreach_tail(SmacqGraphCallback & cb);
  void join_tail(SmacqGraph * newg);
  friend void joincallback::callback(SmacqGraph *g);

  friend SmacqGraph * smacq_graph_add_graph(SmacqGraph * a, SmacqGraph *b) {
    if (!a) return b;
    a->add_graph(b);
    return a;
  }
  
  SmacqGraph * find_branch();
  struct list * list_tails(struct list * list, SmacqGraph * parent, int, int);
  static bool compare_element_names(SmacqGraph * a, SmacqGraph * b);
  static bool compare_elements(SmacqGraph * a, SmacqGraph * b);
  static bool merge_tail_ends(SmacqGraph * a, SmacqGraph * b);
  static void merge_tails(struct list * alist, struct list * blist);
  void merge_all_tails();
  void add_args(SmacqGraph * b);
  static int merge_demuxs(SmacqGraph * a, SmacqGraph * b);
  static int merge_vectors(SmacqGraph * a, SmacqGraph * b);
  static int merge_fanouts(SmacqGraph * a, SmacqGraph * b);
  static int merge_trees(SmacqGraph * a, SmacqGraph * b);
  void optimize_tree();

  /// Iff a module node:
  SmacqGraph * next_graph;

  std::vector<std::vector<SmacqGraph *> > children;
  DynamicArray<SmacqGraph *> parent;
  int numparents;
};

class fanout : public DynamicArray<SmacqGraph *> {};

/// Establish a parent/child relationship with the specified child.
inline void SmacqGraph::add_child(SmacqGraph * newo, unsigned int channel) {
  if (channel >= children.size()) {
    children.resize(channel+1);
  }
  children[channel].push_back(newo);
  newo->add_parent(this);
}

/// Find the highest child that has fanout.
/// NULL is returned if none of our children fan out.
inline SmacqGraph * SmacqGraph::find_branch() {
  assert(children.size());

  if (children[0].size() == 0)
    return NULL;

  if (children.size() == 1 && children[0].size() == 1) 
    return children[0][0]->find_branch();
  
  return this;
}

typedef void foreach_graph_fn(SmacqGraph *);

/* Return a list of all the tails in the given graph (list=NULL initially) */
inline void SmacqGraph::foreach_tail(SmacqGraphCallback & cb) {
  /* A tail is the highest point in the tree that only has linear (fanout = 1) children */
  SmacqGraph * g = this->find_branch();

  if (g) {
    assert(g->children.size() < 2);

    FOREACH_CHILD(g, child->foreach_tail(cb));
  } else {
    cb.callback(this);
  }
}

inline void joincallback::callback(SmacqGraph * g) {
  // Find end of tail
  while(g->children[0].size()) {
    assert(g->children[0].size() == 1);
    g = g->children[0][0];

    // Tails may share some children, so make sure we haven't already done this one
    if (! seen.insert(g).second) {
	return;
    }
  }

  for(SmacqGraph * sg = newg; sg; sg=sg->next_graph) {
    g->add_child(sg); 
  }
}

inline void SmacqGraph::join(SmacqGraph * newg) {
  joincallback j(newg);
  foreach_tail(j);
}


inline void SmacqGraph::add_graph(SmacqGraph * b) {
  SmacqGraph * ap;

  for (ap = this; ap->next_graph; ap=ap->next_graph) ;
  ap->next_graph = b;
}

/// Recursively initalize nodes in graph.
inline void SmacqGraph::init(DTS * dts, SmacqScheduler * sched) {
  for (SmacqGraph * g = this; g; g=g->next_graph) {
	g->init_node_one(dts, sched);
  }
}

inline void SmacqGraph::init_node_one(DTS * dts, SmacqScheduler * sched) {
  struct SmacqModule::smacq_init context;

  context.islast = !(children[0].size());
  context.isfirst = (!numparents);
  context.dts = dts;
  context.self = this;
  context.scheduler = sched;

  this->SmacqGraphNode::init(context);

  FOREACH_CHILD(this, child->init(dts,sched));
}
  

inline SmacqGraph::SmacqGraph(int argc, char ** argv) 
  : next_graph(NULL), children(1), numparents(0)
{
  children.resize(1);
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
      if (this->numparents) {
	this->parent[i] = this->parent[this->numparents];
      }

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
  assert(!algebra.demux && !algebra.vector);
  int size = children[i].size() - 1;

  children[i][j]->remove_parent(this);
  children[i][j] = children[i][size];
  children[i].resize(size);
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
#ifdef SMACQ_DEBUG_GRAPHEDGES
      fprintf(fh, "%*s  n%x -> n%x ;\n", indent, "", this, child);
#endif
      fprintf(fh, "%*s+ Child %d,%d is ", indent, "", i, j);
      count += child->print_one(fh, indent+2);
    });

  return count;
}

inline int SmacqGraph::print(FILE * fh, int indent) {
  double count = 0;
  SmacqGraph * ap;
  for (ap = this; ap; ap=ap->next_graph) {
    ap->print_one(fh, indent);
    count += ap->count_nodes();
  }
  fprintf(fh, "Total number of nodes = %g\n", count);
  return (int)count;
}

inline void SmacqGraph::add_parent(SmacqGraph * parent) {
  this->parent[this->numparents++] = parent;
}		

inline SmacqGraph::~SmacqGraph() {
  /* XXX: Need to handle cycles without double freeing */
  FOREACH_CHILD(this, child = NULL);
  
  if (next_graph)
    delete next_graph;
}

inline void SmacqGraph::downstream_filter_one(smacq_filter_callback_fn callback, void * data) {
  if (!strcmp(name, "where") || !strcmp(name, "equals")) {
	  //fprintf(stderr, "downstream_filters got a known op: %s\n", name);
	  callback(name, argc, argv, data);
  } else {
	  //fprintf(stderr, "don't know anything about %s\n", name);
	  return;
  }

  /* Now do children */
  if (children.size() > 1 || children[0].size() != 1) {
    /* XXX: Lazy for now.  should look for invariants across children */
    return;
  } else {
    children[0][0]->downstream_filter_one(callback, data);
  }
}

inline void SmacqGraph::downstream_filters(smacq_filter_callback_fn callback, void * data) {
  /* XXX: Lazy for now.  should look for invariants across children */
  if (children.size() == 1 && children[0].size() == 1) {
    children[0][0]->downstream_filter_one(callback, data);
  }
}

inline SmacqGraph * SmacqGraph::children_as_heads() {
  // This works only because next_graph isn't used anywhere else 
  // inside a graph.  
  assert(children.size() == 1);

  if (!children[0].size()) return NULL;

  SmacqGraph * g = children[0][0];
  for (unsigned int i = 1; i < children[0].size(); i++) {
    g->next_graph = children[0][i];
    g = g->next_graph;
  }

  g->next_graph = NULL;

  return children[0][0];
}

/// Modify parent(s) and children to replace myself with the specified graph.
inline void SmacqGraph::replace(SmacqGraph * g) {
  parent[0]->print(stderr, 0);

  if (children[0].size()) {
    assert(children.size() == 1);

    SmacqGraph * down = children[0][0];

    for (unsigned int i = 1; i < children[0].size(); i++) {
      children[0][i]->remove_parent(this);
      down->add_graph(children[0][i]);
      children[0][i] = NULL;
    }
    children[0].clear();

    // Join new graph with my children
    g->join(down);
  }
  
  for (int i = 0; i < numparents; i++) {
    parent[i]->replace_child(this, g);
    parent[i]->print(stderr, 4);
    parent[i] = NULL;
  }
  numparents = 0;
}

#endif

