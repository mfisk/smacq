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
  END_SMACQ_OPTIONS
};

typedef FieldVecHash<SmacqGraph*>::iterator OutputsIterator;

SMACQ_MODULE(groupby,
  PROTO_CTOR(groupby);
  PROTO_CONSUME();
  PROTO_PRODUCE();

  SmacqScheduler * sched;
	  
  FieldVec fieldvec;
  SmacqGraph * mastergraph;
  SmacqGraph * children;

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
    partition->init(dts, sched);
    partition->join(children);
    outTable[fieldvec] = partition;
  } 

  return partition;
}

inline void groupbyModule::handle_invalidate(DtsObject datum) {
  OutputsIterator i;

  for (i=outTable.begin(); i != outTable.end(); ++i) {
    if ( i->first.masks(fieldvec)) {
      //fprintf(stderr, "groupby got a partial refresh\n");
      sched->shutdown(i->second);
      outTable.erase(i);
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
    sched->input(get_partition(), datum);
  }

  return SMACQ_FREE;
}

smacq_result groupbyModule::produce(DtsObject & datum, int & outchan) {
  /// Last call
  OutputsIterator i;
  for (i=outTable.begin(); i!= outTable.end(); ++i) {
    sched->shutdown(i->second);
  }
  outTable.clear();
  return SMACQ_END;
}

groupbyModule::groupbyModule(struct SmacqModule::smacq_init * context) 
  : SmacqModule(context), 
    sched(context->scheduler),
    children(context->self->children_as_heads())
{
  int argc;
  char ** argv;

  {
  	smacq_opt ptr;
  	struct smacq_optval optvals[] = {
		{"p", &ptr},
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

	if (ptr.uint32_t) {
		mastergraph = (SmacqGraph*)ptr.uint32_t;
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


