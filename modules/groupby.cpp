#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
#include <fields.h>
#include <bytehash.h>
#include <produceq.h>

#define SMACQ_MODULE_IS_DEMUX 1

#define KEYBYTES 128

static struct smacq_options options[] = {
  {"p", {uint32_t:0}, "pointer to per-partition graph", SMACQ_OPT_TYPE_UINT32},
  {NULL, {string_t:NULL}, NULL, 0}
};

SMACQ_MODULE(groupby,
  PROTO_CTOR(groupby);
  PROTO_PRODUCE();
  PROTO_CONSUME();

  struct fieldset fieldset;
  smacq_graph * mastergraph;
  struct smacq_outputq * outputq;

  struct iovec_hash * hashtable;

  int refresh_type;

  struct output * cont;

  struct iovec * partitionv;

  void destroy_partition(struct output * partition);
  int run_and_queue(struct output* partition);
  int run(struct output* partition);
  void handle_invalidate(DtsObject * datum);
  struct output * get_partition(struct iovec * partitionv);

  static int destroy_partition_wrapper(struct element * key, void * value, void * userdata);
  static int check_invalidate(struct element * key, void * value, void * userdata);
); 

struct output {
  smacq_graph * graph;
  struct runq * runq;
};

/* Bucket may no longer be valid after this call */
inline int groupbyModule::run_and_queue(struct output* partition) {
    int more;
    do {
	DtsObject * product = NULL;
    	more = smacq_sched_iterative_busy(partition->graph, &product, partition->runq, 0);
	/* fprintf(stderr, "run_and_queue produced %p with status %x\n", product, more); */
	if (product) smacq_produce_enqueue(&outputq, product, -1);
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

int groupbyModule::destroy_partition_wrapper(struct element * key, void * value, void * userdata) {
  groupbyModule * ths = (groupbyModule *)userdata;
  struct output * partition = (struct output*)value;

  ths->destroy_partition(partition);

  return 1;
}


/* Bucket may no longer be valid after this call */
inline int groupbyModule::run(struct output* partition) {
    int more;
    DtsObject * product = NULL;
    more = smacq_sched_iterative_busy(partition->graph, &product, partition->runq, 0);
    if (product) smacq_produce_enqueue(&outputq, product, -1);
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

inline struct output * groupbyModule::get_partition(struct iovec * partitionv) {
  struct output * partition;

  partition = (output*)bytes_hash_table_lookupv(hashtable, partitionv, fieldset.num);

  if (partition && !partition->graph) {
    int res = bytes_hash_table_removev(hashtable, partitionv, fieldset.num);
    assert(res);
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
    bytes_hash_table_setv(hashtable, partitionv, fieldset.num, partition);
  } 

  return partition;
}

int groupbyModule::check_invalidate(struct element * key, void * value, void * userdata) {
  groupbyModule * ths = (groupbyModule*)userdata;
  struct element * element = (struct element*)key;
  struct output * partition = (struct output*)value;

  if (bytes_mask(element, ths->partitionv, ths->fieldset.num)) {
    //fprintf(stderr, "groupby got a partial refresh\n");
    ths->destroy_partition(partition);
    return 1;
  }

  return 0;
}

inline void groupbyModule::handle_invalidate(DtsObject * datum) {
   bytes_hash_table_foreach_remove(hashtable, check_invalidate, this);
}

smacq_result groupbyModule::consume(DtsObject * datum, int * outchan) {
  struct output * partition;
  smacq_result status = SMACQ_FREE;

  //fprintf(stderr , "groupby got %p a type %d (refresh is %d)\n", datum, datum->gettype(), refresh_type);

  partitionv = datum->fields2vec(&fieldset);

  if (iovec_has_undefined(partitionv, fieldset.num)) {
  	if (datum->gettype() == refresh_type) {
    		handle_invalidate(datum);
	} else {
		fprintf(stderr, "Fatal Error: Groupby was passed a data object that did not have the fields it is supposed to group by!\n");
		assert(0);
	}
  } else {
     partition = get_partition(partitionv);

     if (datum->gettype() == refresh_type) {
       int res;
       // Handle refresh (kill-off child)
       destroy_partition(partition);
       res = bytes_hash_table_removev(hashtable, partitionv, fieldset.num);
       assert(res);
     } else {
       smacq_sched_iterative_input(partition->graph, datum, partition->runq);
       run(partition);
     }
  }
  
  // Producing input is just a PASS
  if (smacq_produce_peek(&outputq) == datum) {
    smacq_produce_dequeue(&outputq, &datum, outchan);
    status = SMACQ_PASS;
  }

  if (smacq_produce_canproduce(&outputq) || cont) 
	status |= SMACQ_PRODUCE;

  return status;
}

groupbyModule::groupbyModule(struct smacq_init * context) : SmacqModule(context) {
  int argc = 0;
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

  dts->fields_init(&fieldset, argc, argv);
  hashtable = bytes_hash_table_new(KEYBYTES, CHAIN|NOFREE);

  refresh_type = dts->requiretype("refresh");
  if (!mastergraph) {
	  fprintf(stderr, "groupby: error: called without any action to take\n");
	  assert(mastergraph);
  }
}

smacq_result groupbyModule::produce(DtsObject ** datump, int * outchan) {
  int status = 0;
  int lastcall = 1;
  *datump = NULL;

  if (cont) {
    struct output * cont = cont;
    cont = NULL;
    run(cont);

    status = SMACQ_FREE;
    lastcall = 0;
  } 
  
  if (smacq_produce_canproduce(&outputq)) {
    status = smacq_produce_dequeue(&outputq, datump, outchan);
    lastcall = 0;
  }

  if (lastcall) {
    /* This must be last-call, because we didn't ask for it */
    /* Notify all children */
    /* fprintf(stderr, "groupby got last call\n"); */
    bytes_hash_table_foreach_remove(hashtable, destroy_partition_wrapper, this);
    status = smacq_produce_dequeue(&outputq, datump, outchan);
  }

  if (cont) status |= SMACQ_PRODUCE;

  return (smacq_result)(status | smacq_produce_canproduce(&outputq)); 
}

