#ifndef SMACQMODULE_H
#define SMACQMODULE_H

#ifdef __cplusplus

class SmacqModule;

#include <smacq.h>

class SmacqModule {
  public:
	SmacqModule(struct smacq_init * context);
	virtual ~SmacqModule();

	virtual smacq_result consume(DtsObject * datum, int * outchan);
	virtual smacq_result produce(DtsObject ** datump, int * outchan);

	DTS * dts;
};

#else
typedef void SmacqModule;
#endif

#endif	
