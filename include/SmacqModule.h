#ifndef SMACQMODULE_H
#define SMACQMODULE_H

#ifdef __cplusplus

class SmacqModule;

#include <smacq.h>
#include <list>
#include <utility>

/// A virtual base class for SMACQ modules.
class SmacqModule {
  public:
	SmacqModule(struct smacq_init * context);
	virtual ~SmacqModule();

	virtual smacq_result consume(DtsObject datum, int & outchan);
	virtual smacq_result produce(DtsObject & datump, int & outchan);

  protected:
	DTS * dts;

	/// @name Output Queueing
	/// A module can internally queue output objects
	/// @{

	/// Return SMACQ_CANPRODUCE or SMACQ_NONE
	smacq_result canproduce();

	smacq_result dequeue(DtsObject &, int & outchan);
	void enqueue(DtsObject &, int outchan);
	/// @}

 private:
	std::list<std::pair<DtsObject,int> > outputq;
};

inline SmacqModule::SmacqModule(struct smacq_init * context) {
	dts = context->dts;
}

inline SmacqModule::~SmacqModule() {}

inline smacq_result SmacqModule::consume(DtsObject datum, int & outchan) {
	return SMACQ_ERROR;
}

inline smacq_result SmacqModule::produce(DtsObject & datum, int & outchan) {
	if (outputq.empty()) {
		return SMACQ_END;
	} else {
		return dequeue(datum, outchan);
	}
}

inline smacq_result SmacqModule::canproduce() {
	if (outputq.empty()) {
		return SMACQ_NONE;
	} else {
		return SMACQ_PRODUCE;
	}
}
	
inline void SmacqModule::enqueue(DtsObject & datum, int outchan) {
	outputq.push_back(std::pair<DtsObject,int>(datum,outchan));
}

inline smacq_result SmacqModule::dequeue(DtsObject & d, int & outchan) {
  if (outputq.empty()) {
    return SMACQ_NONE;
  }
  
  d = outputq.front().first;
  outchan = outputq.front().second;
  
  outputq.pop_front();
  
  if (outputq.empty()) {
    return SMACQ_PASS;
  } else {
    //fprintf(stderr, "returning SMACQ_PASS|SMACQ_PRODUCE\n");
    return SMACQ_PASS|SMACQ_PRODUCE;
  }
}

#else
typedef void SmacqModule;
#endif

#endif	


