#ifndef SMACQGRAPHNODE_H
#define SMACQGRAPHNODE_H

#include <SmacqModule.h>
#define RINGSIZE 4

/// A node in a SmacqGraph.  Holds instance arguments, meta-data, etc.
class SmacqGraphNode{
 public:
  SmacqGraphNode();
  ~SmacqGraphNode();
  bool set(int argc, char ** argv);
  void init(struct SmacqModule::smacq_init &);

 protected:
  char ** argv;  // set by set()
  int argc; // set by set()
  struct SmacqModule::algebra algebra; // set by load_module()

  // For IterativeScheduler
  bool shutdown;
  bool shutdown_pending;

  SmacqModule * instance;

 private:
  bool load_module();

  // Ring buffer mgmt
  std::vector<DtsObject> q;

#ifndef SMACQ_OPT_NOPTHREADS
  pthread_mutex_t qlock;
  pthread_cond_t ring_notfull;
  pthread_cond_t ring_notempty;

  pthread_t thread;
#endif

  SmacqModule::constructor_fn * constructor;

  GModule * module;

  //struct smacq_options * options;
  //struct smacq_optval * optvals;
};

inline SmacqGraphNode::SmacqGraphNode()
  : shutdown(false), shutdown_pending(false), instance(NULL), q(RINGSIZE) 
{}

inline SmacqGraphNode::~SmacqGraphNode() {
#ifndef SMACQ_OPT_NOPTHREADS
  pthread_mutex_destroy(&qlock);
  pthread_cond_destroy(&ring_notfull);
  pthread_cond_destroy(&ring_notempty);
#endif
}

/// Setup a graph node based on the given argument vector.
inline bool SmacqGraphNode::set(int argc, char ** argv) {
  this->argv = argv;
  this->argc = argc;
  
#ifndef SMACQ_OPT_NOPTHREADS
  smacq_pthread_mutex_init(&this->qlock, NULL);
  smacq_pthread_cond_init(&this->ring_notfull, NULL);
  smacq_pthread_cond_init(&this->ring_notempty, NULL);
#endif

  if (!this->load_module()) {
    exit(-1);
    return false;
  }

  return true;
}

/// Instantiate this node.
inline bool SmacqGraphNode::load_module() {
  struct smacq_functions * modtable;
  
  modtable = (struct smacq_functions*)smacq_find_module(&module, "SMACQ_HOME", "modules", "%s/smacq_%s", "smacq_%s_table", argv[0]);

  if (modtable) {
    constructor = modtable->constructor;
    algebra = modtable->algebra;

    return true;
  } else {
    constructor = NULL;
    
    fprintf(stderr, "Error: unable to find module %s (Need to set %s?)\n", argv[0], "SMACQ_HOME");

    return false;
  }
}


/// Initialize a graph node.  set() must be called first
inline void SmacqGraphNode::init(struct SmacqModule::smacq_init & context) {
  if (instance) return;
  assert(argc && constructor);

  /* fprintf(stderr, "init_modules on graph node %p %s\n", f, f->argv[0]); */

  context.argc = argc;
  context.argv = argv;

  assert(constructor);
  instance = constructor(&context);

  if (! instance) {
     	fprintf(stderr, "Error initializing module %s\n", argv[0]);	 	
	exit(-1);
  }
}

#endif
