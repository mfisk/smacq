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
  char * name;
  char ** argv;
  int argc;
  struct SmacqModule::algebra algebra;

  /// Used by schedulers.
  smacq_result status;
  SmacqModule * instance;

 private:
  bool load_module();

  // Ring buffer mgmt
  std::vector<DtsObject> q;
  int ringsize;
  int ring_produce;
  int ring_consume;
#ifndef SMACQ_OPT_NOPTHREADS
  pthread_mutex_t qlock;
  pthread_cond_t ring_notfull;
  pthread_cond_t ring_notempty;

  pthread_t thread;
#endif

  struct SmacqModule::ops ops;

  GModule * module;

  struct smacq_options * options;
  struct smacq_optval * optvals;
};

inline SmacqGraphNode::SmacqGraphNode()
  : q(RINGSIZE) 
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
  this->name = *argv;
  this->argv = argv;
  this->argc = argc;
  this->q.resize(this->ringsize);
  
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
  
  modtable = (struct smacq_functions*)smacq_find_module(&module, "SMACQ_HOME", "modules", "%s/smacq_%s", "smacq_%s_table", name);

  if (modtable) {
    ops.constructor = modtable->constructor;
    algebra = modtable->algebra;

    return true;
  }
  
  fprintf(stderr, "Error: unable to find module %s (Need to set %s?)\n", name, "SMACQ_HOME");

  return false;
}


/// Initialize a graph node.  It must have already been set().
inline void SmacqGraphNode::init(struct SmacqModule::smacq_init & context) {
  if (instance) return;
  /* fprintf(stderr, "init_modules on graph node %p %s\n", f, f->argv[0]); */

  context.argc = argc;
  context.argv = argv;

  assert(ops.constructor);
  instance = ops.constructor(&context);

  if (! instance) {
     	fprintf(stderr, "Error initializing module %s\n", name);	 	
	exit(-1);
  }
}

#endif
