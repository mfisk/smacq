#include <smacq-internal.h>
#include <stdio.h>
#include <string.h>
#define RINGSIZE 4

static inline void read_module(smacq_graph * module, struct smacq_functions * modtable) {
		module->ops.produce = modtable->produce;
		module->ops.consume = modtable->consume;
		module->ops.shutdown = modtable->shutdown;
		module->ops.init = modtable->init;
		module->ops.thread_fn = modtable->thread;
}

void smacq_graph_print(FILE * fh, smacq_graph * f, int indent) {
  int i;
  if (!f) return;
  
  fprintf(fh, "%p: ", f);

  for (i=0; i<f->argc; i++) {
    fprintf(fh, " %s", f->argv[i]);
  }
  fprintf(fh, "\n");

  for (i=0; i<f->numchildren; i++) {
    fprintf(fh, "%*s+ Child %d is ", indent, "", i);
    smacq_graph_print(fh, f->child[i], indent+2);
  }

}

int smacq_load_module(smacq_graph * module) {
    struct smacq_functions * modtable;

    assert(module);

    if ((modtable = smacq_find_module(&module->module, "SMACQ_HOME", "modules", "%s/smacq_%s", "smacq_%s_table", module->name))) {
		read_module(module, modtable);
		return 1;
    }

    fprintf(stderr, "Error: unable to find module %s (Need to set %s?)\n", module->name, "SMACQ_HOME");
    return 0;
}

static inline void add_parent(smacq_graph * newo, smacq_graph * parent) {
  newo->numparents++;
  newo->parent = g_realloc(newo->parent, newo->numparents * sizeof(smacq_graph *));
  newo->parent[newo->numparents - 1] = parent;
}		

void smacq_destroy_graph(smacq_graph *f) {
  int i;

  /* XXX: Need to handle cycles without double freeing */
  for (i=0; i<f->numchildren; i++) {
    if (f->child[i]) {
      smacq_destroy_graph(f->child[i]);
    }
  }
  smacq_free_module(f);
}


/*
 * Add the module specified by argv[0] as a child of parent.
 * Pass argv,argc to the new module
 */
smacq_graph * smacq_new_module(int argc, char ** argv){
  smacq_graph * newo;

  newo = g_new0(smacq_graph, 1);
  newo->name = *argv;
  newo->argv = argv;
  newo->argc = argc;
  newo->ringsize = RINGSIZE;
  newo->q = g_new0(dts_object *, newo->ringsize);
 
#ifndef SMACQ_OPT_NOPTHREADS
  pthread_mutex_init(&newo->qlock, NULL);
  pthread_cond_init(&newo->ring_notfull, NULL);
  pthread_cond_init(&newo->ring_notempty, NULL);
#endif

  assert(newo);
  if (!smacq_load_module(newo)) {
	exit(-1);
  }
  
  return newo;
}

void smacq_free_module(smacq_graph * f) {
#ifndef SMACQ_OPT_NOPTHREADS
  pthread_mutex_destroy(&f->qlock);
  pthread_cond_destroy(&f->ring_notfull);
  pthread_cond_destroy(&f->ring_notempty);
#endif

  free(f->q);
  free(f->parent);
  free(f->child);
  free(f);
}

int smacq_add_child_only(smacq_graph * parent, smacq_graph * newo) {
    /* Add to dataflow */

    if (parent) {
      newo->previous = parent;
      parent->child = g_realloc(parent->child, (parent->numchildren+2)*sizeof(smacq_graph *));
      parent->child[parent->numchildren++] = newo;
      parent->child[parent->numchildren] = NULL;
      // fprintf(stderr, "Added %s(%p) as child of %s\n", newo->name, newo, parent->name);
      
      add_parent(newo, parent);

      return (parent->numchildren - 1);
    }

    return 0;
}

int smacq_add_child(smacq_graph * parent, smacq_graph * child) {
	int res;
	res = smacq_add_child_only(parent, child);
	add_parent(child, parent);

	return res;
}

smacq_graph * smacq_add_new_child(smacq_graph * parent, int argc, char ** argv){
  smacq_graph * newo = smacq_new_module(argc, argv);
  smacq_add_child_only(parent, newo);
  return newo;
}

smacq_graph * smacq_clone_child(smacq_graph * parent, int child) {
	int i;

	smacq_graph * donor = parent->child[child];
	smacq_graph * new = smacq_add_new_child(parent, donor->argc, donor->argv);

	// Rejoin
	new->child = g_new(smacq_graph *, donor->numchildren);
	for (i=0; i < donor->numchildren; i++) {
		new->child[i] = donor->child[i];
		add_parent(new->child[i], new);
	}

	return new;
}

smacq_graph * smacq_clone_tree(smacq_graph * donorParent, smacq_graph * newParent, int child) {
	int i;

	smacq_graph * donor = donorParent->child[child];
	smacq_graph * new = smacq_add_new_child(newParent, donor->argc, donor->argv);

	new->child = g_new(smacq_graph *, donor->numchildren);
	for (i=0; i < donor->numchildren; i++) {
		new->child[i] = smacq_clone_tree(donor, new, i);
	}

	return new;
}

/*
 * Take an argc, argv specification and build the corresponding
 * dataflow pipeline.  
 * Returns a pointer to the beginning of the dataflow.
 */
smacq_graph * smacq_build_pipeline(int argc, char ** argv) {
  smacq_graph * objs = NULL;
  smacq_graph * last = NULL;

  while(argc > 0) {
    int o_argc = 0;
    char ** o_argv = argv;

    while (argc && strcmp(*argv,"|")) {
      o_argc++;
      argc--, argv++;
    }

    //fprintf(stderr, "adding child %s with %d args\n", o_argv[0], o_argc);
    last = smacq_add_new_child(last, o_argc, o_argv);
    if (!objs) objs = last;

    if (argc && !strcmp(*argv, "|")) {
      argc--, argv++;
      continue;
    }
  }

  return(objs);
}

smacq_graph * smacq_old_build_query(int argc, char ** argv) {
  smacq_graph * objs = NULL;
  smacq_graph * last = NULL;

  assert(0 && "Not implemented!");

  while(argc > 0) {
    int q_argc = 0, w_argc = 0, f_argc=0;
    char ** q_argv = argv;
    char ** w_argv = NULL;
    char ** f_argv = NULL;

    while (argc) {
      if (strcmp(*argv,"from")) {
	q_argc++;

	argc--, argv++;
      } else {
	argc--, argv++;
	break;
      }
    }

    if (argc) f_argv = argv;
      
    /* from ... */
    while (argc) {
      if (strcmp(*argv,"where")) {
	f_argc++;
	argc--, argv++;
      } else {
	*argv = "filter";
	break;
      }
    }

    if (argc) w_argv = argv;

    /* where ... */
    while (argc) {
      w_argc++;
      argc--;
    }

    if (f_argv) {
      last = smacq_add_new_child(last, f_argc, f_argv);
      if (!objs) objs = last;
    }

    { 
      /* Walk through query terms again looking for function calls to handle*/
      int i;
      char * func;
      
      for (i = 1; i < q_argc; i++) {
	if ((func = index(q_argv[i], '('))) {
	  *func = '\0';
	  last = smacq_add_new_child(last, 1, q_argv+i);
	  if (!objs) objs = last;

	}	
      }
    }

    if (w_argv) {
      fprintf(stderr, "where %s\n", w_argv[0]);
      last = smacq_add_new_child(last, w_argc, w_argv);
      if (!objs) objs = last;
    }
      
    last = smacq_add_new_child(last, q_argc, q_argv);
    if (!objs) objs = last;
  }

  return(objs);
}


void smacq_init_modules(smacq_graph * f, smacq_environment * env) {
  struct smacq_init * context = g_new0(struct smacq_init, 1);
  int i;

  if (!f) return;

  context->islast = !f->child;
  context->isfirst = !f->previous;
  context->state = NULL;
  context->env = env;
  context->argc = f->argc;
  context->argv = f->argv;
  context->self = f;
  context->thread_fn = f->ops.thread_fn;
  
  f->ops.init(context);
  f->state = context->state;
  free(context);

  for (i = 0; i < f->numchildren; i++ ) 
    smacq_init_modules(f->child[i], env);
  return;
}



smacq_graph * smacq_graph_add_graph(smacq_graph * a, smacq_graph * b) {
	smacq_graph * ap;
	if (!a) return b;

	for (ap = a; ap->next_graph; ap=ap->next_graph) ;
	ap->next_graph = b;
	return a;
}

static int compare_trees(smacq_graph * a, smacq_graph *b) {
  int i, j;

  if (a==b) return 1;

  if (a->argc != b->argc) return 0;

  for (i=0; i < a->argc; i++) {
    if (strcmp(a->argv[i], b->argv[i])) return 0;
  }

  //fprintf(stderr, "%p(%s) === %p(%s)\n", a, a->name, b, b->name);
  
  for (i = 0; i < b->numchildren; i++) {
    for (j = 0; j < a->numchildren; j++) {
      if (!compare_trees(a->child[j], b->child[i])) {
	return 0;
      }
    }
  }

  return 1;
}

static int merge_tree_tails(smacq_graph * a, smacq_graph *b) {
  int i, j;

  if (compare_trees(a,b)) {
    return 1;
  }

  /* We know they're different, but look for common children */

  for (i = 0; i < b->numchildren; i++) {
    for (j = 0; j < a->numchildren; j++) {
      if (merge_tree_tails(a->child[j], b->child[i])) {
	smacq_add_child(a, b->child[i]);
	add_parent(b->child[i], a);
	// XXX: remove old parent
      }
    }
  }
  
  return 0;
}

static int merge_tree(smacq_graph * a, smacq_graph *b) {
	int i, j;

	if (a==b) return 0;
	if (a->argc != b->argc) return 0;

	for (i=0; i < a->argc; i++) {
	  if (strcmp(a->argv[i], b->argv[i])) return 0;
	}

	// fprintf(stderr, "%p(%s) === %p(%s)\n", a, a->name, b, b->name);

	for (i = 0; i < b->numchildren; i++) {
		/* If any of our twins children are twins of our children, just use that child */
		for (j = 0; j < a->numchildren; j++) {
			if (merge_tree(a->child[j], b->child[i])) {
				b->child[i] = NULL;
				break;
			}
		}

		/* Pawn this child off on our newly found twin */
		if (b->child[i]) {
			smacq_add_child(a, b->child[i]);
			add_parent(b->child[i], a);
			/* fprintf(stderr, "%p child %p(%s) now child of %p(%s)\n", b, 
					b->child[i], b->child[i]->name, 
					a, a->name); */
			// XXX: remove old parent
		}
	}

	return 1;
}

smacq_graph * smacq_merge_graphs(smacq_graph * g) {
  smacq_graph * ap, * bp;
  smacq_graph * candidate;

#ifdef SMACQ_DEBUG
  for (bp = g; bp; bp=bp->next_graph) {
    smacq_graph_print(stderr, bp, 8);
  }
#endif

  /* Merge identical heads */
  for (bp = g, candidate=bp->next_graph; candidate; candidate = bp->next_graph) {
    int merged = 0;
    
    for (ap = g; ap; ap=ap->next_graph) {
      merged = merge_tree(ap, candidate);

      if (merged) {
	break;
      }
    }

    if (merged) {
      /* Remove candidate from the list */
      bp->next_graph = candidate->next_graph;
    } else {
      /* Move down list */
      bp = bp->next_graph;
    }
  }

#warning "Need to do tail optimization on a single tree"

  /* Do the merge again from the tails up */
  for (bp = g; bp && bp->next_graph; bp=bp->next_graph) {
    for (ap = g; ap->next_graph; ap=ap->next_graph) {
      merge_tree_tails(ap, bp->next_graph);
    }
  }

#ifdef SMACQ_DEBUG
  fprintf(stderr, "--- optimized to ---\n");
  for (bp = g; bp; bp=bp->next_graph) {
    smacq_graph_print(stderr, bp, 8);
  }
#endif
  
  return g;
}

