#ifndef SMACQMODULE_H
#define SMACQMODULE_H

#ifdef __cplusplus

class SmacqModule;

#include <smacq.h>
#include <list>
#include <utility>

class SmacqModule {
  public:
	SmacqModule(struct smacq_init * context);
	virtual ~SmacqModule();

	virtual smacq_result consume(DtsObject datum, int * outchan);
	virtual smacq_result produce(DtsObject & datump, int * outchan);

  protected:
	DTS * dts;

	smacq_result canproduce();
	bool dequeue(DtsObject &, int & outchan);
	void enqueue(DtsObject &, int outchan);

	std::list<std::pair<DtsObject,int> > outputq;
};

inline SmacqModule::SmacqModule(struct smacq_init * context) {
	dts = context->dts;
}

inline SmacqModule::~SmacqModule() {}

inline smacq_result SmacqModule::consume(DtsObject datum, int * outchan) {
	return SMACQ_ERROR;
}

inline smacq_result SmacqModule::produce(DtsObject & datum, int * outchan) {
	if (outputq.empty()) {
		return SMACQ_END;
	} else {
		dequeue(datum, *outchan);
		if (outputq.empty()) {
			return SMACQ_PASS|SMACQ_END;
		} else {
			return (smacq_result)(SMACQ_PASS|SMACQ_PRODUCE);
		}
	}
}

inline smacq_result SmacqModule::canproduce() {
	if (outputq.empty()) {
		return (smacq_result)0;
	} else {
		return SMACQ_PRODUCE;
	}
}
	
inline void SmacqModule::enqueue(DtsObject & datum, int outchan) {
	outputq.push_back(std::pair<DtsObject,int>(datum,outchan));
}

inline bool SmacqModule::dequeue(DtsObject & d, int & outchan) {
	if (outputq.empty()) {
		return false;
	}

	d = outputq.front().first;
	outchan = outputq.front().second;

	outputq.pop_front();

	return true;
}

#else
typedef void SmacqModule;
#endif

#endif	


