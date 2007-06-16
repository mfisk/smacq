#ifndef SMACQ_GRAPH_H
#define SMACQ_GRAPH_H
#include <smacq.h>
#include <SmacqModule-interface.h>
#include <DynamicArray.h>
#include <RunQ.h>
#include <set>
#include <vector>
#include <string>
#include <assert.h>
#include <string>

#define DEREF(x) (bind(&SmacqGraphNode_ptr::get, x))

#define FOREACH_CHILD(x, y)						\
   RecursiveLock l((x)->children);                                      \
   for (unsigned int i = 0; i < (x)->children.size(); i ++)             \
     for (unsigned int j = 0; j < (x)->children[i].size(); j ++) {      \
       SmacqGraphNode * child = (x)->children[i][j].get();                  \
       y;                                                               \
    }
/*
#define FOREACH_CHILD(x, y)						\
  RecursiveLock l((x)->children);					\
  for (ThreadSafeVector<ThreadSafeMultiSet<SmacqGraphNode_ptr> >::iterator i = (x)->children.begin();		\
	i != (x)->children.end();					\
	++i)								\
    for (ThreadSafeMultiSet<SmacqGraphNode_ptr>::iterator j = i->begin(); 	\
	j != i->end(); 							\
	++j) 								\
	{								\
      		SmacqGraphNode * child = (*j);				\
      		y;							\
    	}
*/
class DTS;
class SmacqGraphNode;

class Children : public ThreadSafeVector< ThreadSafeMultiSet<SmacqGraphNode_ptr> > {
  // XXX: We really need to provide better interfaces and remove these:
  friend class SmacqGraphNode;
  friend class SmacqGraph;
  friend class SmacqScheduler;

  public:
   typedef ThreadSafeVector< ThreadSafeMultiSet<SmacqGraphNode_ptr> > CONTAINER;

   Children() : CONTAINER(1) {}

   // Can't get this to work:

   template <class CB>
   void foreach(CB cb) {
	using namespace boost::lambda;
 	using namespace std;

	ThreadSafeVector<ThreadSafeMultiSet<SmacqGraphNode_ptr> >::foreach( 
		bind(&ThreadSafeMultiSet<SmacqGraphNode_ptr>::foreach, &_1, protect(cb))
	); 
   }
};


/// This is a container for an SmacqGraphNode which may have multiple heads or tails.
class SmacqGraph {
  friend class SmacqGraphNode;
  friend class SmacqScheduler;

  public:
   
  /// Default CTOR
  SmacqGraph() { }

  /// Construct from a vector of Children
  SmacqGraph(ThreadSafeMultiSet<SmacqGraphNode_ptr> & children); 

  /// Parse a query and add it to a container
  void addQuery(DTS*, SmacqScheduler*, std::string query);
 
  /// This method must be called before the graphs are used.
  void init(DTS *, SmacqScheduler *, bool do_optimize = true);

  /// Shutdown all graphs.
  void shutdown();

  /// Erase container.
  void clear() { 
	head.clear();
  }

  /// Return true iff the container is empty
  bool empty() {
	return head.empty();
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
  void join(SmacqGraphNode *);

  /// Attach the specified graph onto the tail(s) of the graph(s).
  void join(SmacqGraph *, bool dofree=false);

  /// Add a new graph head.
  void add_graph(SmacqGraphNode *);

  /// Add new graph heads.
  void add_graph(SmacqGraph *, bool dofree=false);

  /// Children of the specified graph will also become children of this
  void share_children_of(SmacqGraphNode *);

  /// @}

  /// Recursively clone a graph.  The clone is made a child of
  /// newParent, unless newParent is NULL.
  SmacqGraph * clone(SmacqGraphNode * newParent = NULL);

  /// Add a clone of a graph to this container
  void add_clone(SmacqGraphNode_ptr x, SmacqGraphNode * newParent);

  /// Return a subgraph containing only invariants over the specified field.
  /// The subgraph will contain only boolean filters that are applied to
  /// all objects in the graph (e.g. not within an OR) and that do NOT use 
  /// the specified field.  The returned graph is newly allocated.
  SmacqGraphNode * getInvariants(DTS*, SmacqScheduler*, DtsField&);

  /// Preoptimize graph (unnecessary after init)
  void optimize();

  private:
    ThreadSafeMultiSet<SmacqGraphNode_ptr> head;

    void merge_heads();
    void merge_tails();
    void list_tails(std::set<SmacqGraphNode*> &);

};

#include <ThreadSafe.h>

/// This is a node in a graph.  Each node references its parents and children.
class SmacqGraphNode : private PthreadMutex {
 public:
  /// (Re-)Initialize module
  bool set(int argc, char ** argv);

  /// Return argc
  const int getArgc() const { return argc; }

  /// Return argv (do not modify)
  char ** const getArgv() const { return argv; }

 protected:
  char ** argv;  // set by set()
  int argc; // set by set()
  struct SmacqModule::algebra algebra; // set by load_module()

  ThreadSafeBoolean shutdown, mustProduce;
  SmacqModule * instance;

 private:
  bool load_module();

  // Ring buffer mgmt
  std::vector<DtsObject> q;

  SmacqModule::constructor_fn * constructor;

  lt_dlhandle module;

  //struct smacq_options * options;
  //struct smacq_optval * optvals;

  friend class SmacqScheduler;
  friend class SmacqGraph;

 public:
  SmacqGraphNode(std::string);
  SmacqGraphNode(int argc, char ** argv);

  ~SmacqGraphNode();

  /// This method must be called before a graph is used.
  /// The graph may be modified as a side-effect, so the
  /// caller should replace the called object with the return pointer.
  SmacqGraphNode * init(DTS *, SmacqScheduler *);

  /// @name Parent/Child Relationships
  /// @{

  /// Attach the specified graph onto the tail(s) of the graph(s).
  void join(SmacqGraphNode *);

  /// Attach the specified graph onto the tail(s) of the graph(s).
  void join(SmacqGraph *, bool dofree=false);

  void replace(SmacqGraph *);

  /// Insert a new graph between my parents and me
  void dynamic_insert(SmacqGraphNode *, DTS *);

  /// Add a new graph as one of my children
  void add_child(SmacqGraphNode_ptr child, unsigned int channel = 0);
  void add_children(SmacqGraph * child, unsigned int channel = 0);
  void remove_parent(SmacqGraphNode * parent);
  void remove_child_bynum(int, int);
  void remove_child(SmacqGraphNode_ptr);
  void replace_child(int, int, SmacqGraphNode * newchild);
  void replace_child(int, int, SmacqGraph * newchild);
  void replace_child(SmacqGraphNode * oldchild, SmacqGraphNode * newchild);
  void replace_child(SmacqGraphNode * oldchild, SmacqGraph * newchild);
  static void move_children(SmacqGraphNode * from, SmacqGraphNode * to, bool addvector=false);
  void remove_children();

  bool live_children();
  bool live_parents();

  const std::vector< ThreadSafeMultiSet<SmacqGraphNode_ptr> > getChildren() { return children.snapshot(); }

  /// @}


  /// @name Factories
  /// @{

  /// Construct a new graph using the given arguments.  The new graph
  /// is automatically attached as a child of the current graph.
  SmacqGraphNode * new_child(int argc, char ** argv);

  /// Recursively clone a graph.  The clone is made a child of
  /// newParent, unless newParent is NULL.
  SmacqGraphNode * clone(SmacqGraphNode * newParent);
  /// @}

  /// Print the graph
  void print(FILE * fh, int indent);

  /// Print the graph in re-parsable syntax
  std::string print_query();

  /// Log something about this graph (printf-style arguments)
  void log(const char * format, ...);

  /// @name Invariant Optimization
  /// @{

  /// Return a subgraph containing only invariants over the specified field.
  /// The subgraph will contain only stateless filters that are applied to
  /// all objects in the graph (e.g. not within an OR) and that do NOT use 
  /// the specified field.  The returned graph is newly allocated.
  SmacqGraphNode * getInvariants(DTS*, SmacqScheduler*, DtsField&);

  /// Same as getInvariants() but operates only on the graph's children
  SmacqGraphNode * getChildInvariants(DTS*, SmacqScheduler*, DtsField&);

  /// @}

  /// Shutdown a graph node (will propagate to parents and children).
  /// The node may be destroyed by this call.
  static void do_shutdown(SmacqGraphNode_ptr f);

  /// Attempt to distribute children of this graph.  Return true iff successful.
  bool distribute_children(DTS *);

  /// Queue of input items to consume.
  runq<DtsObject> inputq;

  /// @name Scheduling
  /// @{
 
  /// Schedule the node to produce. 
  void seed_produce();

  /// Schedule the given object to be consumed.
  void runable(DtsObject);

  /// Scheduler is done handling a mustProduce.
  void produce_done();

  /// @}
 private:
  /// Print a node and recurse up.
  std::string print_query_tail();

  SmacqGraph * distribute_rejoin();
  void init_node(DTS *, SmacqScheduler *);
  void init_node_recursively(DTS *, SmacqScheduler *);
  int print_one(FILE * fh, int indent);
  void add_parent(SmacqGraphNode * parent);

  void list_tails(std::set<SmacqGraphNode*> &);
  static bool compare_element_names(SmacqGraphNode * a, SmacqGraphNode * b);
  static bool equiv(SmacqGraphNode * a, SmacqGraphNode * b);
  static bool same_children(SmacqGraphNode * a, SmacqGraphNode * b);
  bool merge_redundant_parents();
  void merge_redundant_children();
  static bool merge_nodes(SmacqGraphNode * a, SmacqGraphNode * b);
  void add_args(SmacqGraphNode * b);

  /// Iff initializied
  SmacqScheduler * scheduler;

  Children children;
  //ThreadSafeVector< ThreadSafeMultiSet<SmacqGraphNode_ptr> > children;

  /// Don't use a refcounted pointer because if we're the only ones that know, it should be GC'd
  ThreadSafeMultiSet<SmacqGraphNode*> parent;

  /// Number of things that can give us input.
  /// This is not a full reference count, but when it decrements to 0
  /// we can clean ourselves up, because nobody should expect to use us.
  /// Children are not included in the refcount and will be notified
  /// when we clean ourselves up.
  ThreadSafeCounter refcount;

  friend void intrusive_ptr_add_ref(SmacqGraphNode *o) { o->refcount.increment(); }
  friend void intrusive_ptr_release(SmacqGraphNode *o);
};

#endif
