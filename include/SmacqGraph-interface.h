#ifndef SMACQ_GRAPH_INTERFACE_H
#define SMACQ_GRAPH_INTERFACE_H
#include <smacq.h>
#include <SmacqModule-interface.h>
#include <SmacqGraphNode.h>
#include <DynamicArray.h>
#include <set>
#include <vector>
#include <assert.h>

#define FOREACH_CHILD(x, y)						\
  for (unsigned int i = 0; i < (x)->children.size(); i ++)		\
    for (unsigned int j = 0; j < (x)->children[i].size(); j ++) {	\
      SmacqGraph * child = (x)->children[i][j].get();			\
      y;								\
    }

class DTS;
class SmacqGraph;

/// A graph of SmacqGraphNode nodes. 
class SmacqGraph : public SmacqGraphNode {
  friend class IterativeScheduler;

 public:
  SmacqGraph(int argc, char ** argv);

  ~SmacqGraph();

  /// This method must be called before a graph is used.
  void init_all(DTS *, SmacqScheduler *, bool do_optimize = true);

  /// @name Parent/Child Relationships
  /// @{

  void replace(SmacqGraph *);

  /// Insert a new graph between my parents and me
  void dynamic_insert(SmacqGraph *, DTS *);

  /// Add a new graph as one of my children
  void add_child(SmacqGraph * child, unsigned int channel = 0);
  void remove_parent(SmacqGraph * parent);
  void remove_child(int, int);
  void remove_child(SmacqGraph *);
  void replace_child(int, int, SmacqGraph * newchild);
  void replace_child(SmacqGraph * oldchild, SmacqGraph * newchild);
  static void move_children(SmacqGraph * from, SmacqGraph * to, bool addvector=false);
  void remove_children();

  bool live_children();
  bool live_parents();

  const std::vector<std::vector<SmacqGraph_ptr> > getChildren() const { return children; }

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

  /// Return a subgraph containing only invariants over the specified field.
  /// The subgraph will contain only stateless filters that are applied to
  /// all objects in the graph (e.g. not within an OR) and that do NOT use 
  /// the specified field.  The returned graph is newly allocated.
  SmacqGraph * getInvariants(DTS*, SmacqScheduler*, DtsField&);

  /// Same as getInvariants() but operates only on the graph's children
  SmacqGraph * getChildInvariants(DTS*, SmacqScheduler*, DtsField&);

  /// @}

  void optimize();

 private:
  void init_node(DTS *, SmacqScheduler *);
  void init_node_recursively(DTS *, SmacqScheduler *);
  int print_one(FILE * fh, int indent);
  void add_parent(SmacqGraph * parent);

  friend SmacqGraph * smacq_graph_add_graph(SmacqGraph * a, SmacqGraph * b) {
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

  /// Iff initializied
  SmacqScheduler * scheduler;

  std::vector<std::vector<SmacqGraph_ptr> > children;

  /// Don't use a refcounted pointer because if we're the only ones that know, it should be GC'd
  DynamicArray<SmacqGraph*> parent;
  int numparents;

  /// Number of things that can give us input.
  /// This is not a full reference count, but when it decrements to 0
  /// we can clean ourselves up, because nobody should expect to use us.
  /// Children are not included in the refcount and will be notified
  /// when we clean ourselves up.
  int refcount;

  friend void intrusive_ptr_add_ref(SmacqGraph *o) { o->refcount++; }
  friend void intrusive_ptr_release(SmacqGraph *o);
};

#endif
