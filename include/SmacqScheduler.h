#ifndef SMACQ_SCHEDULER_H
#define SMACQ_SCHEDULER_H
#include <smacq.h>
#include <RunQ.h>
#include <SmacqGraph.h>
#include <ThreadSafe.h>

/// This is a scheduler for processing any number of SmacqGraph instances.
class SmacqScheduler {
public:
  
  /// A default graph must be specified.  Graph graph's init() method
  /// is called before anything else is done.  
  SmacqScheduler() : debug(false) {}

  ~SmacqScheduler() {
    join_threads();
  }

  /// Set debug output
  void setDebug() { debug = true; }

  /// Get debug status
  bool isDebug() { return debug; }

  /// Cue the head(s) of the given graph to start producing data.
  /// Otherwise data must be provided using the input() method.
  void seed_produce(SmacqGraph*);

  /// Cue the graph to start producing data
  void seed_produce(SmacqGraphNode * startf);

  /// Queue an object for input to the specified graph.
  void input(SmacqGraph * g, DtsObject din);

  /// Run until an output object is ready, and return that object.
  /// Return NULL if execution completes without producing object.
  DtsObject get();

  /// Process a single action or object
  smacq_result decide(SmacqGraphNode *, DtsObject din);

  /// Process a single action or object
  smacq_result decideContainer(SmacqGraph *, DtsObject din);

  /// Run to completion.  
  /// Return false iff error.
  bool busy_loop();

  /// Handle an object produced by a currently running node
  void enqueue(SmacqGraphNode * f, DtsObject d, int outchan);

  /// Handle an object produced by the specified node
  void queue_children(SmacqGraphNode_ptr, DtsObject d, int outchan);

  /// Process a single action or object
  bool element(DtsObject &dout);

  /// Created for the python smacq library
  /// Process a single action or object, if output is produced, return that output.
  DtsObject pyelement();

  /// Return true if the query is done processing
  bool done();

  /// Create some threads to process the current workload.
  /// They will exit when there is nothing to do.
  void start_threads(int numt);

 private:
  /// Place something on the consume queue
  void runable(SmacqGraphNode_ptr f, DtsObject d);

  /// Process a produceq element
  void run_produce(SmacqGraphNode_ptr);

  bool run_consume(SmacqGraphNode_ptr);

  /// Find something to do and do it.
  bool do_something(bool consume_only = false);

  smacq_result decide_children(SmacqGraphNode * g, DtsObject din, int outchan);

  SmacqGraphNode_ptr pop_lock(runq<SmacqGraphNode_ptr> & q);

  void seed_produce_one(SmacqGraphNode*);
  void input_one(SmacqGraphNode*, DtsObject);
  smacq_result decide_set(ThreadSafeMultiSet<SmacqGraphNode_ptr>&, DtsObject);
  bool decide_one(SmacqGraphNode *,  DtsObject);

  bool debug;
  std::vector<pthread_t> threads;
  ThreadSafeCounter Idling;

  void join_threads();
  void thread_loop();

  friend void * iterative_scheduler_thread_start(void * arg) {
        SmacqScheduler * s = (SmacqScheduler*)arg;
        s->thread_loop();
        return NULL;
  }

 public:
  runq<SmacqGraphNode_ptr> consumeq;
  runq<SmacqGraphNode_ptr> produceq;
  runq<DtsObject> outputq;
};

#endif
