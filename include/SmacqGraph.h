/*

We have a really nasty dependency problem here:
SmacqGraph -> SmacqGraphNode -> SmacqModule -> Smacq(Iterative)Scheduler -> RunQ -> SmacqGraph

SmacqModule depends on SmacqScheduler, but only later in the file.  To avoid
a dependency loop, we therefore have to include SmacqModule before anything
else in that chain.

*/

#include <SmacqModule.h>

#ifndef SMACQ_GRAPH_H
#define SMACQ_GRAPH_H

#include <DynamicArray.h>
#include <set>
#include <vector>
#include <assert.h>


#define FOREACH_CHILD(x, y)						\
  for (unsigned int i = 0; i < (x)->children.size(); i ++)		\
    for (unsigned int j = 0; j < (x)->children[i].size(); j ++) {	\
      SmacqGraph * child = (x)->children[i][j];			\
      y;								\
    }

class DTS;
class SmacqGraph;

typedef void smacq_filter_callback_fn(char * operation, int argc, char ** argv, void * data);

#include <SmacqGraphNode.h>

/// A graph of SmacqGraphNode nodes. 
class SmacqGraph : public SmacqGraphNode {
  friend class IterativeScheduler;

 public:
  SmacqGraph(int argc, char ** argv);
  ~SmacqGraph();

  /// This method must be called before a graph is used.
  void init(DTS *, SmacqScheduler *, bool do_optimize = true);

  /// @name Parent/Child Relationships
  /// @{

  void replace(SmacqGraph *);

  void add_child(SmacqGraph * child, unsigned int channel = 0);
  void remove_parent(SmacqGraph * parent);
  void remove_child(int, int);
  void remove_child(SmacqGraph *);
  void replace_child(int, int, SmacqGraph * newchild);
  void replace_child(SmacqGraph * oldchild, SmacqGraph * newchild);
  static void move_children(SmacqGraph * from, SmacqGraph * to, bool addvector=false);

  bool live_children();
  bool live_parents();

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

  /// Children of the specified graph will also become children of this
  void share_children_of(SmacqGraph *);

  /// @}

  /// @name Factories
  /// @{

  /// Parse a query and construct a new graph to execute it 
  static SmacqGraph * newQuery(DTS*, SmacqScheduler*, int argc, char ** argv);

  /// Construct a new graph using the given arguments.  The new graph
  /// is automatically attached as a child of the current graph.
  SmacqGraph * new_child(int argc, char ** argv);

  /// Recursively clone a graph.  The clone is made a child of
  /// newParent, unless newParent is NULL.
  SmacqGraph * clone(SmacqGraph * newParent);
  /// @}

  double count_nodes();
  int print(FILE * fh, int indent);

  /// @name Invariant Optimization
  /// @{

  /// Callback function is called for each downstream filter.	
  void downstream_filters(smacq_filter_callback_fn callback, void * data);

  /// Return a subgraph containing only invariants over the specified field.
  /// The subgraph will contain only stateless filters that are applied to
  /// all objects in the graph (e.g. not within an OR) and that do NOT use 
  /// the specified field.  The returned graph is newly allocated.
  SmacqGraph * get_invariants_over_field(DTS*, SmacqScheduler*, DtsField&);

  /// @}

  void optimize();

 private:
  void downstream_filter_one(smacq_filter_callback_fn callback, void * data);
  void init_node_one(DTS *, SmacqScheduler *);
  int print_one(FILE * fh, int indent);
  void add_parent(SmacqGraph * parent);

  friend SmacqGraph * smacq_graph_add_graph(SmacqGraph * a, SmacqGraph *b) {
    if (!a) return b;
    a->add_graph(b);
    return a;
  }
  
  void list_tails(std::set<SmacqGraph*> &);
  void list_tails_recurse(std::set<SmacqGraph*> &list);
  static bool compare_element_names(SmacqGraph * a, SmacqGraph * b);
  static bool equiv(SmacqGraph * a, SmacqGraph * b);
  static bool same_children(SmacqGraph * a, SmacqGraph * b);
  bool merge_redundant_parents();
  void merge_redundant_children();
  void merge_tails();
  void merge_heads();
  static bool merge_nodes(SmacqGraph * a, SmacqGraph * b);
  void add_args(SmacqGraph * b);

  /// Iff a module node:
  SmacqGraph * next_graph;

  std::vector<std::vector<SmacqGraph *> > children;
  DynamicArray<SmacqGraph *> parent;
  int numparents;
};

class fanout : public DynamicArray<SmacqGraph *> {};

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
inline void SmacqGraph::list_tails(std::set<SmacqGraph*> &list) {
  for(SmacqGraph * sg = this; sg; sg=sg->next_graph) {
  	sg->list_tails_recurse(list);
  }
}

/// Recursively initialize a list of all the tails in this given graph
inline void SmacqGraph::list_tails_recurse(std::set<SmacqGraph*> &list) {
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

  std::set<SmacqGraph*> list;
  std::set<SmacqGraph*>::iterator i;
  list_tails(list);

  for(i = list.begin(); i != list.end(); ++i) {
    for(SmacqGraph * sg = newg; sg; sg=sg->next_graph) {
      (*i)->add_child(sg); 
    }
  }
}

inline void SmacqGraph::add_graph(SmacqGraph * b) {
  SmacqGraph * ap;

  for (ap = this; ap->next_graph; ap=ap->next_graph) ;
  ap->next_graph = b;
}

/// Recursively initalize nodes in graph.
inline void SmacqGraph::init(DTS * dts, SmacqScheduler * sched, bool do_optimize) {
  for (SmacqGraph * g = this; g; g=g->next_graph) {
    g->init_node_one(dts, sched);
  }

  if (do_optimize) optimize();
}

inline void SmacqGraph::init_node_one(DTS * dts, SmacqScheduler * sched) {
  struct SmacqModule::smacq_init context;

  context.islast = !(children[0].size());
  context.isfirst = (!numparents);
  context.dts = dts;
  context.self = this;
  context.scheduler = sched;

  this->SmacqGraphNode::init(context);

  FOREACH_CHILD(this, child->init(dts, sched));
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
  if (!strcmp(argv[0], "where") || !strcmp(argv[0], "equals")) {
    //fprintf(stderr, "downstream_filters got a known op: %s\n", argv[0]);
    callback(argv[0], argc, argv, data);
  } else {
    //fprintf(stderr, "don't know anything about %s\n", argv[0]);
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

inline void SmacqGraph::share_children_of(SmacqGraph * g) {
  assert(g->children.size() == 1);
  if (!g->children[0].size()) return;

  for (unsigned int i = 0; i < g->children[0].size(); i++) {
    join(g->children[0][i]);
  }
}

/// Modify parent(s) and children to replace myself with the specified graph.
inline void SmacqGraph::replace(SmacqGraph * g) {
  //parent[0]->print(stderr, 0);

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
    //parent[i]->print(stderr, 4);
    parent[i] = NULL;
  }
  numparents = 0;
}

inline SmacqGraph * SmacqGraph::get_invariants_over_field(DTS * dts, SmacqScheduler * sched, DtsField & field) {
  SmacqGraph * more;
  if (!algebra.stateless) {
    //fprintf(stderr, "%s is not stateless, so stopping invariant search\n", argv[0]);
    return NULL;
  }

  if (children.size() == 1 && children[0].size() == 1) {
    more = children[0][0]->get_invariants_over_field(dts, sched, field);
  } else {
    more = NULL;
  }

  if (!instance) 
    init(dts, sched);

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


#endif

