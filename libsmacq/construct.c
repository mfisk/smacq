#include <flow-internal.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#define RINGSIZE 4


int flow_load_module(struct filter * module) {
    struct smacq_functions * modtable;
    void * self;
    char buf[1024];

    assert(module);
    assert(module->name);
    snprintf(buf, 1024, "smacq_%s_table", module->name);
    self = dlopen(NULL, RTLD_NOW);
    if (!self) {
	fprintf(stderr, "Warning: %s\n", dlerror());
	return 0;
    } else {
    	modtable = dlsym(self, buf);
	if (modtable) {
		// fprintf(stderr, "Info: found module %s internally\n", module->name);
		module->produce = modtable->produce;
		module->consume = modtable->consume;
		module->shutdown = modtable->shutdown;
		module->init = modtable->init;
		module->thread_fn = modtable->thread;
		return 1;
	}
    }

    // Find a shared library
    {
      char modfile[256];
      snprintf(modfile, 256, "/%s/smacq_%s.so", getenv("SMACQ_HOME"), module->name);

      if (! (module->module = g_module_open(modfile, 0))) {
         fprintf(stderr, "%s: %s (Need to set SMACQ_HOME?)\n", module->name, g_module_error());
         return 0;
      }
    }

    g_module_symbol(module->module, "smacq_options", (gpointer)&module->options);
    
    if (g_module_symbol(module->module, buf, (gpointer)&modtable)) {
	    	module->produce = modtable->produce;
		module->consume = modtable->consume;
		module->init = modtable->init;
		module->shutdown = modtable->shutdown;
		module->thread_fn = modtable->thread;

		return 1;
    }

    // Old style:
    if (!g_module_symbol(module->module, "flow_produce", (gpointer)&module->produce)) {
      fprintf(stderr, "Unable to resolve flow_produce in %s: %s\n", module->name, g_module_error());
	return 0;
    }
      
    if (!g_module_symbol(module->module, "flow_consume", (gpointer)&module->consume)) {
      fprintf(stderr, "Unable to resolve flow_consume in %s: %s\n", module->name, g_module_error());
      return 0;
    }
      
    if (!g_module_symbol(module->module, "flow_init", (gpointer)&module->init)) {
      fprintf(stderr, "Unable to resolve flow_init in %s: %s\n", module->name, g_module_error());
      return 0;
    }

    g_module_symbol(module->module, "flow_shutdown", (gpointer)&module->shutdown);

    return 1;
}

static inline void add_parent(struct filter * newo, struct filter * parent) {
  newo->numparents++;
  newo->parent = g_realloc(newo->parent, newo->numparents * sizeof(struct filter *));
  newo->parent[newo->numparents - 1] = parent;
}		

/*
 * Add the module specified by argv[0] as a child of parent.
 * Pass argv,argc to the new module
 */
struct filter * smacq_add_child(struct filter * parent, int argc, char ** argv){
  struct filter * newo;

  newo = g_new0(struct filter, 1);
  newo->name = *argv;
  newo->argv = argv;
  newo->argc = argc;
  newo->previous = parent;
  newo->ringsize = RINGSIZE;
  newo->q = g_new0(dts_object *, newo->ringsize);
  
  pthread_mutex_init(&newo->qlock, NULL);
  pthread_cond_init(&newo->ring_notfull, NULL);
  pthread_cond_init(&newo->ring_notempty, NULL);

  assert(newo);
  if (!flow_load_module(newo)) {
	exit(-1);
  }
    /* Add to dataflow */

    if (parent) {
	parent->next = g_realloc(parent->next, (parent->numchildren+2)*sizeof(struct filter *));
	parent->next[parent->numchildren++] = newo;
	parent->next[parent->numchildren] = NULL;
	// fprintf(stderr, "Added %s(%p) as child of %s\n", newo->name, newo, parent->name);

	add_parent(newo, parent);
    }

    return newo;
}

struct filter * smacq_clone_child(struct filter * parent, int child) {
	int i;

	struct filter * donor = parent->next[child];
	struct filter * new = smacq_add_child(parent, donor->argc, donor->argv);

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
	struct filter * new = smacq_add_child(newParent, donor->argc, donor->argv);

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
    last = smacq_add_child(last, o_argc, o_argv);
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
      last = smacq_add_child(last, f_argc, f_argv);
      if (!objs) objs = last;
    }

    { 
      /* Walk through query terms again looking for function calls to handle*/
      int i;
      char * func;
      
      for (i = 1; i < q_argc; i++) {
	if ((func = index(q_argv[i], '('))) {
	  *func = '\0';
	  last = smacq_add_child(last, 1, q_argv+i);
	  if (!objs) objs = last;

	}	
      }
    }

    if (w_argv) {
      fprintf(stderr, "where %s\n", w_argv[0]);
      last = smacq_add_child(last, w_argc, w_argv);
      if (!objs) objs = last;
    }
      
    last = smacq_add_child(last, q_argc, q_argv);
    if (!objs) objs = last;
  }

  return(objs);
}


void flow_init_modules(struct filter * f, smacq_environment * env) {
  struct flow_init * context = g_new0(struct flow_init, 1);
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
    flow_init_modules(f->next[i], env);
  return;
}


