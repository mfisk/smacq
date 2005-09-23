#ifndef ITERATIVE_SCHEDULER_INTERFACE_H
#define ITERATIVE_SCHEDULER_INTERFACE_H
#include <smacq.h>
#include <RunQ.h>
#include <set>

/// This is currently the only scheduler implementation.
class IterativeScheduler {
public:
  
  /// A default graph must be specified.  Graph graph's init() method
  /// is called before anything else is done.  Iff produce_first is
  IterativeScheduler() {};

  /// Cue the head(s) of the given graph to start producing data.
  /// Otherwise data must be provided using the input() method.
  void seed_produce(SmacqGraph*);

  /// Queue an object for input to the specified graph.
  void input(SmacqGraph * g, DtsObject din);

  /// Perform an immediate shutdown for the specified graph.  The argument is invalidated.
  void do_shutdown(SmacqGraph * f);

  /// Run until an output object is ready.
  smacq_result get(DtsObject &dout);

  /// Process a single action or object
  smacq_result IterativeScheduler::decide(SmacqGraph *, DtsObject din);

  /// Run to completion.  
  /// Return false iff error.
  bool busy_loop();

  /// Handle an object produced by a currently running node
  void enqueue(SmacqGraph * f, DtsObject d, int outchan);

 private:
  struct ConsumeItem {
    SmacqGraph_ptr g;
    DtsObject d;
   
    // Constructor 
    ConsumeItem() {};
  
    // Copy constructor
    ConsumeItem(ConsumeItem & old) : g(old.g), d(old.d) {};

    // The runq class will set to NULL to remove references
    ConsumeItem(void * v) { assert(!v);};
  };

  /// Place something on the consume queue
  void runable(SmacqGraph *f, DtsObject d);

  /// Process a produceq element
  smacq_result run_produce(SmacqGraph * f);

  /// Process a consumeq element
  void run_consume(ConsumeItem & i);

  /// Process a single action or object
  smacq_result element(DtsObject &dout);

  /// Handle an object produced by the specified node
  void queue_children(SmacqGraph * f, DtsObject d, int outchan);

  smacq_result decide_children(SmacqGraph * g, DtsObject din, int outchan);

  runq<struct ConsumeItem> consumeq;
  runq<SmacqGraph_ptr> produceq;
  runq<DtsObject> outputq;

  std::set<SmacqGraph*> enqueue_stack;
};

#endif
