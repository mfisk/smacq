#include <smacq.h>
#include <smacq-dataflow.h>
#include <dts.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <SmacqModule.h>

#define RINGSIZE 4
#define FIRST(a,b) ((a) ? (a) : (b))

static inline void read_module(smacq_graph * graph, struct smacq_functions * modtable) {
		graph->ops.constructor = modtable->constructor;
		graph->algebra = modtable->algebra;
}

double smacq_graph_count_nodes(smacq_graph * f) {
  int i;
  double count = 1;
  if (!f) return 0;

  for (i=0; i<f->numchildren; i++) {
    count += smacq_graph_count_nodes(f->child[i]);
  }

  if (f->numparents) {
	  return count / f->numparents;
  }
  return count;
}

int smacq_graph_print(FILE * fh, smacq_graph * f, int indent) {
  int i;
  int count = 1;

  if (!f) return 0;

  fprintf(fh, "%p: ", f);

  for (i=0; i<f->argc; i++) {
    fprintf(fh, " %s", f->argv[i]);
  }
  fprintf(fh, "\n");

  for (i=0; i<f->numchildren; i++) {
#ifdef SMACQ_DEBUG_GRAPHEDGES
    fprintf(fh, "%*s  n%x -> n%x ;\n", indent, "", f, f->child[i]);
#endif
    fprintf(fh, "%*s+ Child %d is ", indent, "", i);
    count += smacq_graph_print(fh, f->child[i], indent+2);
  }

  return count;
}

int smacq_graphs_print(FILE * fh, smacq_graph * g, int indent) {
  double count = 0;
  smacq_graph * ap;
  for (ap = g; ap; ap=ap->next_graph) {
    smacq_graph_print(fh, ap, indent);
    count += smacq_graph_count_nodes(ap);
  }
  fprintf(fh, "Total number of nodes = %g\n", count);
  return (int)count;
}

int smacq_load_module(smacq_graph * graph) {
    struct smacq_functions * modtable;

    assert(graph);

    if ((modtable = (struct smacq_functions*)smacq_find_module(&graph->module, "SMACQ_HOME", "modules", "%s/smacq_%s", "smacq_%s_table", graph->name))) {
		read_module(graph, modtable);
		return 1;
    }

    fprintf(stderr, "Error: unable to find module %s (Need to set %s?)\n", graph->name, "SMACQ_HOME");
    return 0;
}

void smacq_add_parent(smacq_graph * newo, smacq_graph * parent) {
  newo->numparents++;
  newo->parent = (smacq_graph**)g_realloc(newo->parent, newo->numparents * sizeof(smacq_graph *));
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

/* Recursively clone a standalone graph */
smacq_graph * smacq_graph_clone(DTS * dts, smacq_graph * orig) {
  int i;

  smacq_graph * clone = new smacq_graph;
  *clone = *orig;

  /* Need our own input buffer */
  clone->q.resize(clone->ringsize);

  /* Have distinct children */
  clone->numparents = 0;
  clone->numchildren = 0;
  clone->parent = NULL;
  clone->child = NULL;

  for(i=0; i < orig->numchildren; i++) {
      if (orig->child[i]) {
	  smacq_add_child(clone, smacq_graph_clone(dts, orig->child[i]));
	  /* fprintf(stderr, "clone %p got child %p\n", clone, clone->child[clone->numchildren-1]); */
      } else {
	  assert(0);
      }
  }

  return clone;
}

/*
 * Add the module specified by argv[0] as a child of parent.
 * Pass argv,argc to the new module
 */
smacq_graph * smacq_new_module(int argc, char ** argv){
  smacq_graph * newo;

  newo = new smacq_graph;
  newo->name = *argv;
  newo->argv = argv;
  newo->argc = argc;
  newo->ringsize = RINGSIZE;
  newo->q.resize(newo->ringsize);
 
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

  free(f->parent);
  free(f->child);

  delete f;
}

int smacq_add_child_only(smacq_graph * parent, smacq_graph * newo) {
    /* Add to dataflow */

    if (parent) {
      newo->previous = parent;
      parent->child = (smacq_graph**)g_realloc(parent->child, (parent->numchildren+2)*sizeof(smacq_graph *));
      parent->child[parent->numchildren++] = newo;
      parent->child[parent->numchildren] = NULL;
      // fprintf(stderr, "Added %s(%p) as child of %s\n", newo->name, newo, parent->name);
      
      return (parent->numchildren - 1);
    }

    return 0;
}

int smacq_add_child(smacq_graph * parent, smacq_graph * child) {
	int res;
	res = smacq_add_child_only(parent, child);

	if (parent) 
		smacq_add_parent(child, parent);

	return res;
}

smacq_graph * smacq_add_new_child(smacq_graph * parent, int argc, char ** argv){
  smacq_graph * newo = smacq_new_module(argc, argv);
  smacq_add_child(parent, newo);
  return newo;
}

smacq_graph * smacq_clone_child(smacq_graph * parent, int child) {
	int i;

	smacq_graph * donor = parent->child[child];
	smacq_graph * newg = smacq_add_new_child(parent, donor->argc, donor->argv);

	// Rejoin
	newg->child = g_new(smacq_graph *, donor->numchildren);
	for (i=0; i < donor->numchildren; i++) {
		newg->child[i] = donor->child[i];
		smacq_add_parent(newg->child[i], newg);
	}

	return newg;
}

smacq_graph * smacq_clone_tree(smacq_graph * donorParent, smacq_graph * newParent, int child) {
	int i;

	smacq_graph * donor = donorParent->child[child];
	smacq_graph * newg = smacq_add_new_child(newParent, donor->argc, donor->argv);

	newg->child = g_new(smacq_graph *, donor->numchildren);
	for (i=0; i < donor->numchildren; i++) {
		newg->child[i] = smacq_clone_tree(donor, newg, i);
	}

	return newg;
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

void downstream_filters(smacq_graph * mod, smacq_filter_callback_fn callback, void * data) {
  if (!strcmp(mod->name, "where") || !strcmp(mod->name, "equals")) {
	  //fprintf(stderr, "downstream_filters got a known op: %s\n", mod->name);
	  callback(mod->name, mod->argc, mod->argv, data);
  } else {
	  //fprintf(stderr, "don't know anything about %s\n", mod->name);
	  return;
  }

  /* Now do children */
  if (mod->numchildren > 1) {
	  /* XXX: Lazy for now.  should look for invariants across children */
	  return;
  }
  if (mod->numchildren == 0) {
	  return;
  }
  downstream_filters(mod->child[0], callback, data);
}

void smacq_downstream_filters(smacq_graph * mod, smacq_filter_callback_fn callback, void * data) {
  /* XXX: Lazy for now.  should look for invariants across children */
  if (mod->numchildren == 1) {
  	downstream_filters(mod->child[0], callback, data);
  }
}


void smacq_init_modules(smacq_graph * f, DTS * dts) {
  struct smacq_init context;
  int i;

  if (!f || f->instance) return;
  /* fprintf(stderr, "init_modules on graph node %p %s\n", f, f->argv[0]); */

  context.islast = !f->child;
  context.isfirst = !f->previous;
  context.dts = dts;
  context.argc = f->argc;
  context.argv = f->argv;
  context.self = f;

  f->instance = f->ops.constructor(&context);
  assert(f->ops.constructor);

  if (! f->instance) {
     	fprintf(stderr, "Error initializing module %s\n", f->name);	 	
	exit(-1);
  }

  for (i = 0; i < f->numchildren; i++ ) {
    smacq_init_modules(f->child[i], dts);
  }

  return;
}

smacq_graph * smacq_graph_add_graph(smacq_graph * a, smacq_graph * b) {
	smacq_graph * ap;
	if (!a) return b;

	for (ap = a; ap->next_graph; ap=ap->next_graph) ;
	ap->next_graph = b;
	return a;
}

void smacq_remove_parent(smacq_graph * child, const smacq_graph * parent) {
  int i;
  if (!parent) return;

  assert(child->numparents);

  for (i = 0; i < child->numparents; i++) {
    if (child->parent[i] == parent) {
      child->numparents--;

      if (child->numparents) {
	child->parent[i] = child->parent[child->numparents];
      }

      break;
    }
  }
}

void smacq_replace_child(smacq_graph * parent, int num, smacq_graph * newchild) {
  assert(num < parent->numchildren);

  if (parent->child[num] == newchild) return;
  assert(parent->child[num] != newchild);

  //fprintf(stderr, "About to replace %p (child %d of %p) with %p\n", parent->child[num], num, parent, newchild);
  smacq_remove_parent(parent->child[num], parent);
  smacq_add_parent(newchild, parent);
  parent->child[num] = newchild;
}

void smacq_remove_child(smacq_graph * a, int num) {
  assert(num < a->numchildren);
  assert(!a->algebra.demux && !a->algebra.vector);

  smacq_remove_parent(a->child[num], a);

  a->child[num] = NULL;
  a->numchildren--;

  if (a->numchildren) {
    /* Still children left */

    /* Move last child to replace this one */
    a->child[num] = a->child[a->numchildren];
  }
}
