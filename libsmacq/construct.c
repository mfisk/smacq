#include <smacq-internal.h>
#include <stdio.h>
#include <string.h>
#define RINGSIZE 4

static inline void read_module(struct filter * module, struct smacq_functions * modtable) {
		module->produce = modtable->produce;
		module->consume = modtable->consume;
		module->shutdown = modtable->shutdown;
		module->init = modtable->init;
		module->thread_fn = modtable->thread;
}

int smacq_load_module(struct filter * module) {
    struct smacq_functions * modtable;

    assert(module);

    if ((modtable = smacq_find_module(&module->module, "SMACQ_HOME", "modules", "%s/smacq_%s", "smacq_%s_table", module->name))) {
		read_module(module, modtable);
		return 1;
    }

    fprintf(stderr, "Error: unable to find module %s (Need to set %s?)\n", module->name, "SMACQ_HOME");
    return 0;
}

static inline void add_parent(struct filter * newo, struct filter * parent) {
  newo->numparents++;
  newo->parent = g_realloc(newo->parent, newo->numparents * sizeof(struct filter *));
  newo->parent[newo->numparents - 1] = parent;
}		

void smacq_destroy_graph(struct filter *f) {
  int i;

  /* XXX: Need to handle cycles without double freeing */
  for (i=0; i<f->numchildren; i++) {
    if (f->next[i]) {
      smacq_destroy_graph(f->next[i]);
    }
  }
  smacq_free_module(f);
}


/*
 * Add the module specified by argv[0] as a child of parent.
 * Pass argv,argc to the new module
 */
struct filter * smacq_new_module(int argc, char ** argv){
  struct filter * newo;

  newo = g_new0(struct filter, 1);
  newo->name = *argv;
  newo->argv = argv;
  newo->argc = argc;
  newo->ringsize = RINGSIZE;
  newo->q = g_new0(dts_object *, newo->ringsize);
  
  pthread_mutex_init(&newo->qlock, NULL);
  pthread_cond_init(&newo->ring_notfull, NULL);
  pthread_cond_init(&newo->ring_notempty, NULL);

  assert(newo);
  if (!smacq_load_module(newo)) {
	exit(-1);
  }
  
  return newo;
}

void smacq_free_module(struct filter * f) {
  pthread_mutex_destroy(&f->qlock);
  pthread_cond_destroy(&f->ring_notfull);
  pthread_cond_destroy(&f->ring_notempty);

  free(f->q);
  free(f->parent);
  free(f->next);
  free(f);
}

int smacq_add_child(struct filter * parent, struct filter * newo) {
    /* Add to dataflow */

    if (parent) {
      newo->previous = parent;
      parent->next = g_realloc(parent->next, (parent->numchildren+2)*sizeof(struct filter *));
      parent->next[parent->numchildren++] = newo;
      parent->next[parent->numchildren] = NULL;
      // fprintf(stderr, "Added %s(%p) as child of %s\n", newo->name, newo, parent->name);
      
      add_parent(newo, parent);

      return (parent->numchildren - 1);
    }

    return 0;
}

struct filter * smacq_add_new_child(struct filter * parent, int argc, char ** argv){
  struct filter * newo = smacq_new_module(argc, argv);
  smacq_add_child(parent, newo);
  return newo;
}

struct filter * smacq_clone_child(struct filter * parent, int child) {
	int i;

	struct filter * donor = parent->next[child];
	struct filter * new = smacq_add_new_child(parent, donor->argc, donor->argv);

	// Rejoin
	new->next = g_new(struct filter *, donor->numchildren);
	for (i=0; i < donor->numchildren; i++) {
		new->next[i] = donor->next[i];
		add_parent(new->next[i], new);
	}

	return new;
}

struct filter * smacq_clone_tree(struct filter * donorParent, struct filter * newParent, int child) {
	int i;

	struct filter * donor = donorParent->next[child];
	struct filter * new = smacq_add_new_child(newParent, donor->argc, donor->argv);

	new->next = g_new(struct filter *, donor->numchildren);
	for (i=0; i < donor->numchildren; i++) {
		new->next[i] = smacq_clone_tree(donor, new, i);
	}

	return new;
}

/*
 * Take an argc, argv specification and build the corresponding
 * dataflow pipeline.  
 * Returns a pointer to the beginning of the dataflow.
 */
struct filter * smacq_build_pipeline(int argc, char ** argv) {
  struct filter * objs = NULL;
  struct filter * last = NULL;

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

struct filter * smacq_build_query(int argc, char ** argv) {
  struct filter * objs = NULL;
  struct filter * last = NULL;

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


void smacq_init_modules(struct filter * f, smacq_environment * env) {
  struct smacq_init * context = g_new0(struct smacq_init, 1);
  int i;

  if (!f) return;

  context->islast = !f->next;
  context->isfirst = !f->previous;
  context->state = NULL;
  context->env = env;
  context->argc = f->argc;
  context->argv = f->argv;
  context->self = f;
  context->thread_fn = f->thread_fn;
  
  f->init(context);
  f->state = context->state;
  free(context);

  for (i = 0; i < f->numchildren; i++ ) 
    smacq_init_modules(f->next[i], env);
  return;
}


