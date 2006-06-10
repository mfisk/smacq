#ifndef ITERATIVE_SCHEDULER_INTERFACE_H
#define ITERATIVE_SCHEDULER_INTERFACE_H
#include <smacq.h>
#include <RunQ.h>
#include <SmacqGraph-interface.h>
#include <ThreadSafe.h>

/// This is currently the only scheduler implementation.
class IterativeScheduler {
public:
  
  /// A default graph must be specified.  Graph graph's init() method
  /// is called before anything else is done.  Iff produce_first is
  IterativeScheduler(int cpus = 0) : debug(false) {
	if (cpus) slave_threads(cpus);
  }

  ~IterativeScheduler() {
	join_threads();
  }

  /// Set debug output
  void setDebug() { debug = true; }

  /// Get debug status
  bool isDebug() { return debug; }

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
  smacq_result decideContainer(SmacqGraphContainer *, DtsObject din);

  /// Run to completion.  
  /// Return false iff error.
  bool busy_loop();

  /// Handle an object produced by a currently running node
  void enqueue(SmacqGraph * f, DtsObject d, int outchan);

  /// Handle an object produced by the specified node
  void queue_children(SmacqGraph_ptr, DtsObject d, int outchan);

  /// Process a single action or object
  smacq_result element(DtsObject &dout);

 private:
  /// Place something on the consume queue
  void runable(SmacqGraph_ptr f, DtsObject d);

  /// Process a produceq element
  void run_produce(SmacqGraph_ptr);

  bool run_consume(SmacqGraph_ptr);

  /// Find something to do and do it.
  bool do_something(bool consume_only = false);

  /// Create some threads to process the current workload.
  /// They will exit when there is nothing to do.
  void slave_threads(int numt);

  smacq_result decide_children(SmacqGraph * g, DtsObject din, int outchan);

  SmacqGraph_ptr pop_lock(runq<SmacqGraph_ptr> & q);

  void seed_produce_one(SmacqGraph*);
  void input_one(SmacqGraph*, DtsObject);
  smacq_result decide_set(ThreadSafeMultiSet<SmacqGraph_ptr>&, DtsObject);
  bool decide_one(SmacqGraph *,  DtsObject);

  bool debug;
  std::vector<pthread_t> threads;

  void join_threads();
  void thread_loop();
  bool done();

  friend void * iterative_scheduler_thread_start(void * arg) {
        SmacqScheduler * s = (SmacqScheduler*)arg;
        s->thread_loop();
        return NULL;
  }

 public:
  runq<SmacqGraph_ptr> consumeq;
  runq<SmacqGraph_ptr> producefirstq;
  runq<SmacqGraph_ptr> produceq;
  runq<DtsObject> outputq;
};

#endif
