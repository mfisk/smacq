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

 private:
  struct ConsumeItem {
    SmacqGraph_ptr g;
    DtsObject d;
   
    ConsumeItem() : g(NULL), d(NULL) {};
 
    // The runq class will set to NULL to remove references
    ConsumeItem(void * v) : g(NULL), d(NULL) { assert(!v);};
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

  runq<ConsumeItem> consumeq;
  runq<SmacqGraph_ptr> produceq;
  runq<DtsObject> outputq;

  std::set<SmacqGraph*> enqueue_stack;
};

#endif
