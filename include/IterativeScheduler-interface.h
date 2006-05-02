#ifndef ITERATIVE_SCHEDULER_INTERFACE_H
#define ITERATIVE_SCHEDULER_INTERFACE_H
#include <smacq.h>
#include <RunQ.h>
#include <set>
#include <SmacqGraph-interface.h>

/// This is currently the only scheduler implementation.
class IterativeScheduler {
public:
  
  /// A default graph must be specified.  Graph graph's init() method
  /// is called before anything else is done.  Iff produce_first is
  IterativeScheduler() {};

  /// Cue the head(s) of the given graph to start producing data.
  /// Otherwise data must be provided using the input() method.
  void seed_produce(SmacqGraphContainer*);

  /// Cue the graph to start producing data
  void seed_produce(SmacqGraph * startf);

  /// Queue an object for input to the specified graph.
  void input(SmacqGraphContainer * g, DtsObject din);

  /// Run until an output object is ready.
  smacq_result get(DtsObject &dout);

  /// Process a single action or object
  smacq_result decide(SmacqGraph *, DtsObject din);

  /// Process a single action or object
  smacq_result decide(SmacqGraphContainer *, DtsObject din);

  /// Run to completion.  
  /// Return false iff error.
  bool busy_loop();

  /// Handle an object produced by a currently running node
  void enqueue(SmacqGraph * f, DtsObject d, int outchan);

  /// Handle an object produced by the specified node
  void queue_children(SmacqGraph * f, DtsObject d, int outchan);

  /// Process a single action or object
  smacq_result element(DtsObject &dout);

 private:
  /// Place something on the consume queue
  void runable(SmacqGraph_ptr f, DtsObject d);

  /// Process a produceq element
  void run_produce(SmacqGraph * f);

  /// Process a consumeq element.  Return true iff something could be done.
  bool run_consume();

  /// Find something to do and do it.
  bool do_something();

  smacq_result decide_children(SmacqGraph * g, DtsObject din, int outchan);

  runq<SmacqGraph_ptr> consumeq;
  runq<SmacqGraph_ptr> producefirstq;
  runq<SmacqGraph_ptr> produceq;
  runq<DtsObject> outputq;

  std::set<SmacqGraph*> enqueue_stack;
};

#endif
