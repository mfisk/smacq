#ifndef SMACQMODULEWRAPPER_H
#define SMACQMODULEWRAPPER_H

#include <SmacqModule.h>

struct state;

class SmacqModuleWrapper : public SmacqModule {
  public:
        SmacqModuleWrapper(smacq_graph *, smacq_init *);
	smacq_result produce(DtsObject**, int*);
	smacq_result consume(DtsObject*, int*);
        ~SmacqModuleWrapper();

  private:
        struct smacq_module_ops ops;
	struct state * state;
};

#endif
