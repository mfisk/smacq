#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define SMACQ_MODULE_IS_DEMUX 1
#include <SmacqModule.h>
#include <FieldVec.h>
#include <SmacqGraph.h>
#include <SmacqScheduler.h>

#define KEYBYTES 128

static struct smacq_options options[] = {
  {"p", {uint32_t:0}, "pointer to per-partition graph", SMACQ_OPT_TYPE_UINT32},
  {"g", {boolean_t:0}, "show per-partition graph", SMACQ_OPT_TYPE_BOOLEAN},
  END_SMACQ_OPTIONS
};

typedef FieldVecHash<SmacqGraph*>::iterator OutputsIterator;

SMACQ_MODULE(groupby,
  PROTO_CTOR(groupby);
  PROTO_DTOR(groupby);
  PROTO_CONSUME();

  SmacqScheduler * sched;
	  
  FieldVec fieldvec;
  SmacqGraph * mastergraph;
  SmacqGraph * self;

  FieldVecHash<SmacqGraph*> outTable;

  int refresh_type;

  void handle_invalidate(DtsObject datum);
  SmacqGraph * get_partition();
); 

inline SmacqGraph * groupbyModule::get_partition() {
  SmacqGraph * partition;

  partition = outTable[fieldvec];
  if (!partition) {
    partition = mastergraph->clone(NULL);
    partition->share_children_of(self);
    partition->init(dts, sched);
    outTable[fieldvec] = partition;
  } 

  return partition;
}

/// Delete any tables that fit the fieldvec mask
inline void groupbyModule::handle_invalidate(DtsObject datum) {
  OutputsIterator i, prev;

  for (i=outTable.begin(); i != outTable.end();) {
    if (i->first.masks(fieldvec)) {
      sched->shutdown(i->second);
      prev = i++;
      outTable.erase(prev);
/*
	/// Iterator invalidated; so start over
      	handle_invalidate(datum);
      	return;
*/

    } else {
      ++i;
    }
  }
}

smacq_result groupbyModule::consume(DtsObject datum, int & outchan) {
  //fprintf(stderr , "groupby got %p a type %d (refresh is %d)\n", 
  //datum.get(), datum->gettype(), refresh_type);

  fieldvec.getfields(datum);

  if (datum->gettype() == refresh_type) {
    handle_invalidate(datum);
  } else {
    SmacqGraph * p = get_partition();
    sched->input(p, datum);
  }

  return SMACQ_FREE;
}

groupbyModule::~groupbyModule() {
  OutputsIterator i;
  for (i=outTable.begin(); i!= outTable.end(); ++i) {
    sched->shutdown(i->second);
  }
  outTable.clear();
}

groupbyModule::groupbyModule(struct SmacqModule::smacq_init * context) 
  : SmacqModule(context), 
    sched(context->scheduler),
    self(context->self)
{
  int argc;
  char ** argv;

  {
  	smacq_opt ptr, print_graph;
  	struct smacq_optval optvals[] = {
		{"p", &ptr},
		{"g", &print_graph},
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

	if (ptr.uint32_t) {
		mastergraph = (SmacqGraph*)ptr.uint32_t;
		if (print_graph.boolean_t) 
			mastergraph->print(stderr, 15);
	}

  }

  fieldvec.init(dts, argc, argv);

  refresh_type = dts->requiretype("refresh");
  if (!mastergraph) {
	  fprintf(stderr, "groupby: error: called without any action to take\n");
	  assert(mastergraph);
  }
}


