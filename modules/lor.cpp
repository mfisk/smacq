#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
#include <smacq.h>
#include <FieldVec.h>
#include <IoVec.h>

static struct smacq_options options[] = {
  END_SMACQ_OPTIONS
};

#define SMACQ_MODULE_IS_STATELESS 1

SMACQ_MODULE(lor,
  PROTO_CONSUME();
  PROTO_CTOR(lor);
  PROTO_DTOR(lor);

  struct clause * clause;
  int num_clauses;
  int num_active_clauses;

  int build_clause(char ** argv, int num);
); 

struct clause {
  struct runq * runq;
  smacq_graph * graph;
  int isnot;
};

int lorModule::build_clause(char ** argv, int num) {
		  /* End of query */
		  smacq_graph * g;
		  int isnot = 0;

		  //fprintf(stderr, "Build clause from %s(%p)+%d ... %s\n", argv[0], argv, num, argv[num-1]);

		  if (num >= 1 && !strcmp(argv[0], "not")) {
			  isnot = 1;
			  num--, argv++;
		  }
		  if (num < 1) {
			  fprintf(stderr, "OR: empty clause\n");
			  return 0;
		  }

		  num_clauses++;
		  clause = (struct clause*)realloc(clause, num_clauses * sizeof(struct clause));
		  g = smacq_build_query(dts, num, argv);
		  if (0 != smacq_start(g, ITERATIVE, dts)) {
			  return 0;
		  } 

		  clause[num_clauses-1].graph = g;
		  clause[num_clauses-1].runq = NULL;
		  clause[num_clauses-1].isnot = isnot;

		  smacq_sched_iterative_init(g, &clause[num_clauses-1].runq, 0);

		  return 1;
}

smacq_result lorModule::consume(DtsObject datum, int * outchan) {
  int i;
  smacq_result status = SMACQ_FREE;

  for (i=0; i < num_clauses; i++) {
  	DtsObject output = NULL;
	struct clause * j = &clause[i];
	smacq_result more = 0;

	if (!j) continue;

	smacq_sched_iterative_input(j->graph, datum, j->runq);
	more = smacq_sched_iterative_busy(j->graph, &output, j->runq, 0);

	//more = smacq_sched_iterative(j->graph, datum, &output, &j->runq, 0);

	//fprintf(stderr, "Passed in %p to graph %d, got %p out:\n", datum, i, output);

	if (SMACQ_END & more) {
		fprintf(stderr, "%d ended\n", i);
		free(j);
		clause[i].graph = NULL;
		num_active_clauses--;
		break;
	}


	if (output == datum) {
		if (clause[i].isnot) {
			status = SMACQ_FREE;
		} else {
			status = SMACQ_PASS;
			break;
		}
	} else if (output) {
		fprintf(stderr, "Error OR called on non-boolean function!\n");
	} else if (clause[i].isnot) {
		status = SMACQ_PASS;
		break;
	}
  }

  if (num_active_clauses < 1) {
	status |= SMACQ_END;
  }
  return status;
}

lorModule::lorModule(struct smacq_init * context) : SmacqModule(context) {
  int argc = context->argc-1;
  char ** argv = context->argv+1;

  {
  	int start = 0;
	int i;
  	for (i=0; i<argc; i++) {
	  if (!strcmp(argv[i], ";")) {
		  if (!build_clause(argv+start, i - start))
			  assert(0);
		  start = i+1;
	  }
	}
	if (start < argc) {
		build_clause(argv+start, argc - start);
	}
  }

  num_active_clauses = num_clauses;

  //fprintf(stderr, "init done with %d clauses\n", num_active_clauses);
}

lorModule::~lorModule() {
  int i;
  for (i=0; i<num_clauses; i++) {
	  struct clause * j = &clause[i];
	  if (j) {
		  smacq_sched_iterative_shutdown(j->graph, j->runq);
	  }
  }
}

