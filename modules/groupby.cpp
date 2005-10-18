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

typedef FieldVecHash<SmacqGraphContainer*>::iterator OutputsIterator;

SMACQ_MODULE(groupby,
  PROTO_CTOR(groupby);
  PROTO_DTOR(groupby);
  PROTO_CONSUME();

  FieldVec fieldvec;
  SmacqGraphContainer * mastergraph;
  SmacqGraph * self;

  FieldVecHash<SmacqGraphContainer*> outTable;

  int refresh_type;

  void handle_invalidate(DtsObject datum);
  SmacqGraphContainer * get_partition();
); 

inline SmacqGraphContainer * groupbyModule::get_partition() {
  SmacqGraphContainer * partition = outTable[fieldvec];

  if (!partition) {
    partition = mastergraph->clone(NULL);
    partition->share_children_of(self);
    partition->init(dts, scheduler, false); // Already optimized
    //fprintf(stderr, "new partition instance:\n");
    //partition->print(stderr, 30);
    outTable[fieldvec] = partition;
  } 

  return partition;
}

/// Delete any tables that fit the fieldvec mask
inline void groupbyModule::handle_invalidate(DtsObject datum) {
  OutputsIterator i, prev;

  for (i=outTable.begin(); i != outTable.end();) {
    if (i->first.masks(fieldvec)) {
      i->second->shutdown();
      delete i->second;
      prev = i++;
      outTable.erase(prev);
    } else {
      ++i;
    }
  }
}

smacq_result groupbyModule::consume(DtsObject datum, int & outchan) {
  // fprintf(stderr , "groupby got %p a type %d (refresh is %d)\n", datum.get(), datum->gettype(), refresh_type);

  if (! fieldvec.getfields(datum)) return SMACQ_FREE;

  if (datum->gettype() == refresh_type) {
    handle_invalidate(datum);
  } else {
    SmacqGraphContainer * p = get_partition();
    scheduler->input(p, datum);
  }

  return SMACQ_FREE;
}

groupbyModule::~groupbyModule() {
  // fprintf(stderr, "groupby: shutddown\n");
  OutputsIterator i;
  for (i=outTable.begin(); i != outTable.end(); ++i) {
    // fprintf(stderr, "groupby: shutddown %p\n", i->second);
    i->second->shutdown();
    delete i->second;
  }
  outTable.clear();

  delete mastergraph;
}

groupbyModule::groupbyModule(struct SmacqModule::smacq_init * context) 
  : SmacqModule(context), self(context->self)
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
		mastergraph = (SmacqGraphContainer*)ptr.uint32_t;
		if (print_graph.boolean_t) 
			mastergraph->print(stderr, 15);
		mastergraph->optimize();  // optimize now, before cloning
	}

  }

  fieldvec.init(dts, argc, argv);

  refresh_type = dts->requiretype("refresh");
  if (!mastergraph) {
	  fprintf(stderr, "groupby: error: called without any action to take\n");
	  assert(mastergraph);
  }
}


