#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
#include <FieldVec.h>
#include <IoVec.h>
//#include <produceq.h>

#define SMACQ_MODULE_IS_DEMUX 1

#define KEYBYTES 128

static struct smacq_options options[] = {
  {"p", {uint32_t:0}, "pointer to per-partition graph", SMACQ_OPT_TYPE_UINT32},
  END_SMACQ_OPTIONS
};

typedef IoVecHash<struct output*>::iterator OutputsIterator;

SMACQ_MODULE(groupby,
  PROTO_CTOR(groupby);
  PROTO_PRODUCE();
  PROTO_CONSUME();

  FieldVec fieldvec;
  smacq_graph * mastergraph;

  IoVecHash<struct output*> outTable;

  int refresh_type;

  struct output * cont;

  void destroy_partition(struct output * partition);
  smacq_result run_and_queue(struct output* partition);
  smacq_result run(struct output* partition);
  void handle_invalidate(DtsObject datum);
  struct output * get_partition();
); 

struct output {
  smacq_graph * graph;
  struct runq * runq;
};

/* Bucket may no longer be valid after this call */
inline smacq_result groupbyModule::run_and_queue(struct output* partition) {
    smacq_result more;
    do {
	DtsObject product = NULL;
    	more = smacq_sched_iterative_busy(partition->graph, &product, partition->runq, 0);
	/* fprintf(stderr, "run_and_queue produced %p with status %x\n", product, more); */
	if (product) enqueue(product, -1);
    	if (more & SMACQ_END) {
      		assert(!partition->runq);
      		smacq_destroy_graph(partition->graph);
      		partition->graph = NULL;
		break;
    	}
    } while (more);
    return more;
}

inline void groupbyModule::destroy_partition(struct output * partition) {
  //fprintf(stderr, "doing shutdown of %p\n", partition->graph);
  smacq_sched_iterative_shutdown(partition->graph, partition->runq);
  run_and_queue(partition);
}


/* Bucket may no longer be valid after this call */
inline smacq_result groupbyModule::run(struct output* partition) {
    smacq_result more;
    DtsObject product = NULL;
    more = smacq_sched_iterative_busy(partition->graph, &product, partition->runq, 0);
    if (product) enqueue(product, -1);
    if (more & SMACQ_END) {
      assert(!partition->runq);
      smacq_destroy_graph(partition->graph);
      partition->graph = NULL;
    }
    if (more) {
      assert(!cont);
      cont = partition;
    }

    return more;
}

inline struct output * groupbyModule::get_partition() {
  struct output * partition;

  partition = outTable[fieldvec];

  if (partition && !partition->graph) {
    /* run() already ended this graph, but we didn't know about it before. */ 
    free(partition);
    partition = NULL;
  }

  if (!partition) {
    partition =  g_new0(struct output, 1);
    partition->graph = smacq_graph_clone(dts, mastergraph);
    /*
    fprintf(stderr, "New partition %p (graph = %p)\n", partition, partition->graph);
    */

    smacq_start(partition->graph, ITERATIVE, dts);
    smacq_sched_iterative_init(partition->graph, &partition->runq, 0);
    outTable[fieldvec] = partition;
  } 

  return partition;
}

inline void groupbyModule::handle_invalidate(DtsObject datum) {
  OutputsIterator to_erase;
  bool do_erase = false;
  OutputsIterator i;
  for (i=outTable.begin(); i != outTable.end(); ++i) {
    if (do_erase) {
      outTable.erase(to_erase);
      do_erase = false;
    }

    if ( i->first.masks(fieldvec)) {
      //fprintf(stderr, "groupby got a partial refresh\n");
      destroy_partition(i->second);
      to_erase = i;
      do_erase = true;
    }
  }

  if (do_erase) {
    outTable.erase(to_erase);
  }

}

smacq_result groupbyModule::consume(DtsObject datum, int * outchan) {
  struct output * partition;
  smacq_result status = SMACQ_FREE;

  //fprintf(stderr , "groupby got %p a type %d (refresh is %d)\n", datum, datum->gettype(), refresh_type);

  fieldvec.getfields(datum);

  if (fieldvec.has_undefined()) {
  	if (datum->gettype() == refresh_type) {
    		handle_invalidate(datum);
	} else {
		fprintf(stderr, "Fatal Error: Groupby was passed a data object that did not have the fields it is supposed to group by!\n");
		assert(0);
	}
  } else {
     partition = get_partition();

     if (datum->gettype() == refresh_type) {
       // Handle refresh (kill-off child)
       destroy_partition(partition);
       outTable.erase(fieldvec);

     } else {
       smacq_sched_iterative_input(partition->graph, datum, partition->runq);
       run(partition);
     }
  }
  
  // Producing input is just a PASS
  if (outputq.front().first == datum) {
    dequeue(datum, *outchan);
    status = SMACQ_PASS;
  }

  if (canproduce() || cont) 
	status |= SMACQ_PRODUCE;

  return status;
}

groupbyModule::groupbyModule(struct smacq_init * context) : SmacqModule(context) {
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
		mastergraph = (smacq_graph*)ptr.uint32_t;
	}

  }

  fieldvec.init(dts, argc, argv);

  refresh_type = dts->requiretype("refresh");
  if (!mastergraph) {
	  fprintf(stderr, "groupby: error: called without any action to take\n");
	  assert(mastergraph);
  }
}

smacq_result groupbyModule::produce(DtsObject & datump, int * outchan) {
  smacq_result status = 0;
  int lastcall = 1;
  datump = NULL;

  if (cont) {
    struct output * cont = cont;
    cont = NULL;
    run(cont);

    status = SMACQ_FREE;
    lastcall = 0;
  } 
  
  if (canproduce()) {
    status = dequeue(datump, *outchan);
    lastcall = 0;
  }

  if (lastcall) {
    /* This must be last-call, because we didn't ask for it */
    /* Notify all children */
    /* fprintf(stderr, "groupby got last call\n"); */
    IoVecHash<struct output*>::iterator i;
    for (i = outTable.begin(); i!=outTable.end(); i++) {
      destroy_partition(i->second);
      // outTable.erase(i); // Don't bother --- about to delete table anyway
    }
    status = dequeue(datump, *outchan);
  }

  if (cont) status |= SMACQ_PRODUCE;

  return (smacq_result)(status | canproduce()); 
}

