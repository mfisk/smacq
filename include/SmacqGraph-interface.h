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

template <class T>
class PointerVector : public std::vector<T> {
  public:
        void erase(unsigned int i) {
           // Swap, drop, and roll...
           unsigned int last = std::vector<T>::size() - 1;
           if (i < last) {
                (*this)[i] = (*this)[last];
	   }
           (*this)[last] = NULL;
           std::vector<T>::pop_back();
        }
};

class SmacqGraphContainer {
  friend class SmacqGraph;
  friend class IterativeScheduler;

  public:
   
  /// Default CTOR
  SmacqGraphContainer() { }

  /// Construct from a vector of Children
  SmacqGraphContainer(PointerVector<SmacqGraph_ptr> & children); 
 
  /// This method must be called before the graphs are used.
  void init(DTS *, SmacqScheduler *, bool do_optimize = true);

  /// Shutdown all graphs.
  void shutdown();

  /// Erase container.
  void clear() { 
	head.clear();
  }

  /// Print the graphs
  void print(FILE * fh, int indent);

  /// Print the graph in re-parsable syntax
  std::string print_query();

  /// @name Combining Graphs
  /// A container can have multiple heads and tails and
  /// may even be disconnected. 
  /// @{
  
  /// Attach the specified graph onto the tail(s) of the graph(s).
  void join(SmacqGraph *);

  /// Attach the specified graph onto the tail(s) of the graph(s).
  void join(SmacqGraphContainer *, bool dofree=false);

  /// Add a new graph head.
  void add_graph(SmacqGraph *);

  /// Add new graph heads.
  void add_graph(SmacqGraphContainer *, bool dofree=false);

  /// Children of the specified graph will also become children of this
  void share_children_of(SmacqGraph *);

  /// @}

  /// Recursively clone a graph.  The clone is made a child of
  /// newParent, unless newParent is NULL.
  SmacqGraphContainer * clone(SmacqGraph * newParent);

  /// Return a subgraph containing only invariants over the specified field.
  /// The subgraph will contain only boolean filters that are applied to
  /// all objects in the graph (e.g. not within an OR) and that do NOT use 
  /// the specified field.  The returned graph is newly allocated.
  SmacqGraph * getInvariants(DTS*, SmacqScheduler*, DtsField&);

  /// Preoptimize graph (unnecessary after init)
  void optimize();

  private:
    PointerVector<SmacqGraph_ptr> head;

    void merge_heads();
    void merge_tails();
    void list_tails(std::set<SmacqGraph*> &);

};

/// A graph of SmacqGraphNode nodes. 
class SmacqGraph : public SmacqGraphNode {
  friend class IterativeScheduler;
  friend class SmacqGraphContainer;

 public:
  SmacqGraph(int argc, char ** argv);

  ~SmacqGraph();

  /// This method must be called before a graph is used.
  /// The graph may be modified as a side-effect, so the
  /// caller should replace the called object with the return pointer.
  SmacqGraph * init(DTS *, SmacqScheduler *);

  /// @name Parent/Child Relationships
  /// @{

  /// Attach the specified graph onto the tail(s) of the graph(s).
  void join(SmacqGraph *);

  /// Attach the specified graph onto the tail(s) of the graph(s).
  void join(SmacqGraphContainer *, bool dofree=false);

  void replace(SmacqGraphContainer *);

  /// Insert a new graph between my parents and me
  void dynamic_insert(SmacqGraph *, DTS *);

  /// Add a new graph as one of my children
  void add_child(SmacqGraph * child, unsigned int channel = 0);
  void add_child(SmacqGraphContainer * child, unsigned int channel = 0);
  void remove_parent(SmacqGraph * parent);
  void remove_child(int, int);
  void remove_child(SmacqGraph *);
  void replace_child(int, int, SmacqGraph * newchild);
  void replace_child(int, int, SmacqGraphContainer * newchild);
  void replace_child(SmacqGraph * oldchild, SmacqGraph * newchild);
  void replace_child(SmacqGraph * oldchild, SmacqGraphContainer * newchild);
  static void move_children(SmacqGraph * from, SmacqGraph * to, bool addvector=false);
  void remove_children();

  bool live_children();
  bool live_parents();

  const std::vector<PointerVector<SmacqGraph_ptr> > getChildren() const { return children; }

  /// @}


  /// @name Factories
  /// @{

  /// Parse a query and construct a new graph to execute it 
  static SmacqGraphContainer * newQuery(DTS*, SmacqScheduler*, int argc, char ** argv);

  /// Construct a new graph using the given arguments.  The new graph
  /// is automatically attached as a child of the current graph.
  SmacqGraph * new_child(int argc, char ** argv);

  /// Recursively clone a graph.  The clone is made a child of
  /// newParent, unless newParent is NULL.
  SmacqGraph * clone(SmacqGraph * newParent);
  /// @}

  /// Print the graph
  void print(FILE * fh, int indent);

  /// Print the graph in re-parsable syntax
  std::string print_query();

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

  /// Shutdown a graph node (will propagate to parents and children).
  /// The node may be destroyed by this call.
  static void do_shutdown(SmacqGraph * f);

  /// Attempt to distribute children of this graph.  Return true iff successful.
  bool distribute_children(DTS *);

 private:
  /// Print a node and recurse up.
  std::string print_query_tail();

  SmacqGraphContainer * distribute_rejoin();
  void init_node(DTS *, SmacqScheduler *);
  void init_node_recursively(DTS *, SmacqScheduler *);
  int print_one(FILE * fh, int indent);
  void add_parent(SmacqGraph * parent);

  void list_tails(std::set<SmacqGraph*> &);
  static bool compare_element_names(SmacqGraph * a, SmacqGraph * b);
  static bool equiv(SmacqGraph * a, SmacqGraph * b);
  static bool same_children(SmacqGraph * a, SmacqGraph * b);
  bool merge_redundant_parents();
  void merge_redundant_children();
  static bool merge_nodes(SmacqGraph * a, SmacqGraph * b);
  void add_args(SmacqGraph * b);

  /// Iff initializied
  SmacqScheduler * scheduler;

  std::vector<PointerVector<SmacqGraph_ptr> > children;

  /// Don't use a refcounted pointer because if we're the only ones that know, it should be GC'd
  PointerVector<SmacqGraph*> parent;

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
