#include <smacq-parser.h>

dts_environment * parse_tenv;

struct vphrase newvphrase(char * verb, struct arglist * args) {
  	struct vphrase vphrase;
	vphrase.verb = verb;
	vphrase.args = args;
	if (!strcmp(verb, "select")) {
     		/* SQL's select is really a projection operation */
     		vphrase.verb = "project";
	}
	return vphrase;
}

void arglist2argv(struct arglist * alist, int * argc, char *** argvp) {
	int i;
	char ** argv;
	struct arglist * al;

	for(i=0, al=alist; al; i++) {
		al=al->next;
	}
	argv = malloc(sizeof(char **) * i);
	*argc = i;
	*argvp = argv;

	for(i=0, al=alist; i<*argc; i++, al=al->next) {
		argv[i] = al->arg;
	}
}

char * arglist2str(struct arglist * alist) {
	char * argstr;
	struct arglist * al;
	int len = 1;

	for(al=alist; al; al=al->next) 
		len += strlen(al->arg) + 1;

	argstr = malloc(len);

	for(al=alist; al; al=al->next) {
		strcatn(argstr, len, al->arg);
		strcatn(argstr, len, " ");
	}

	return argstr;
}

void graph_join(struct graph * graph, struct graph newg) {
	if (!newg.head) 
		return; /* Do nothing */

	assert(graph);

	if (!graph->head) {
		graph->head = newg.head;
		graph->tail = newg.tail;
		return;
	}

	/* fprintf(stderr, "Adding %s after %s\n", newg.head->name, graph->tail->name); */

	/* Splice them together */
	assert(graph->tail);

	smacq_add_child(graph->tail, newg.head); 
	graph->tail = newg.tail;
}
	
void graph_append(struct graph * graph, smacq_graph * newmod) {
	if (graph->tail) 
		smacq_add_child(graph->tail, newmod); 
	graph->tail = newmod;
	if (! graph->head) 
		graph->head = newmod;
}

struct arglist * arglist_append(struct arglist * tail, struct arglist * addition) {
	for (; tail->next; tail=tail->next) ;
	tail->next = addition;
	for (; tail->next; tail=tail->next) ;

	return tail;
}
	
struct graph newgroup(struct group group, struct vphrase vphrase) {
  /*
   * This function violates some abstractions by knowing the 
   * calling syntax for "groupby" and constructing arguments for it.
   */
  struct arglist * atail;
  struct graph g = { NULL, NULL};
  struct arglist * ap;
  int argcont = 0;
  
  if (!group.args) { 
    /* Do nothing if this was "group by" NULL */
    return newmodule(vphrase.verb, vphrase.args);
  }
  
  atail = arglist_append(group.args, newarg("--", 0, NULL));
  
  /* Insert function operations */
  for(ap=vphrase.args; ap; ap=ap->next) {
    /* fprintf(stderr, "group arg %s isfunc = %d\n", ap->arg, ap->isfunc); */
    if (ap->isfunc) {
      if (argcont) 
	atail = arglist_append(atail, newarg("|", 0, NULL));
      atail = arglist_append(atail, newarg(ap->arg, 0, NULL));
      atail = arglist_append(atail, ap->func_args);
      ap->isfunc = 0;
      argcont = 1;
    }
  }
  if (argcont) 
    g = newmodule("groupby", group.args);
  
  if (group.having) 
    graph_join(&g, newmodule("filter", group.having));
  
  graph_join(&g, newmodule(vphrase.verb, vphrase.args));
  
  return g;
}


struct graph newmodule(char * module, struct arglist * alist) {
     struct arglist * anew;
     struct graph graph = { head: NULL, tail: NULL };

     int argc;
     char ** argv;

     int rename_argc = 1;
     char ** rename_argv = NULL;
     struct arglist * ap;

     anew = newarg(module, 0, NULL);
     arglist_append(anew, alist);

     for(ap=anew; ap; ap=ap->next) {
        /* Check for rename options on arguments */
     	if (ap->rename) {
		rename_argc += 2;
		rename_argv = realloc(rename_argv, rename_argc * sizeof(char*));
		rename_argv[rename_argc - 2] = ap->arg;
		rename_argv[rename_argc - 1] = ap->rename;
		ap->arg = ap->rename;
	}

	/* Check for function arguments */
	if (ap->isfunc) 
		graph_join(&graph, newmodule(ap->arg, ap->func_args));
     }

     if (rename_argc > 1) {
        /* We need to splice in a rename module before this module */
     	rename_argv[0] = "rename";
        graph_append(&graph, smacq_new_module(rename_argc, rename_argv));
     }

     arglist2argv(anew, &argc, &argv);
     graph_append(&graph, smacq_new_module(argc, argv));

     return graph;
}

struct arglist * newarg(char * arg, int isfunc, struct arglist * func_args) {
     struct arglist * al = calloc(1, sizeof(struct arglist));
     al->arg = arg;
     if (isfunc) {
     	al->func_args = func_args;
	al->isfunc = 1;
     }

     return(al);
}

char * opstr(dts_comparison * comp) {
  switch (comp->op) {
  case LT:
    return "<";

  case GT:
    return ">";

  case EQ:
    return "==";

  case GEQ:
    return ">=";

  case LEQ:
    return "<=";

  case NEQ:
    return "!=";

  case LIKE:
    return "like";

  case EXIST:
    return "";

  case FUNC:
    return "[FUNC]";

 case AND:
    return "AND";

 case OR:
    return "OR";
  }

  return "[ERR]";
}


char * print_comparison(dts_comparison * comp) {
  int size = 20;
  char * buf;
  dts_comparison * c;
  char * b;
  int i;

  if (comp->fieldname) 
    size += strlen(comp->fieldname);

  if (comp->valstr)
  	size += strlen(comp->valstr);

  if (comp->op == FUNC)
	  for (i=0; i<comp->argc; i++)
    		size += strlen(comp->argv[i])+2;

  buf = malloc(size);

  if (comp->op == EXIST) {
    snprintf(buf, size, "(%s)", comp->fieldname);
  } else if (comp->op == FUNC) {
    snprintf(buf, size, "%s(", comp->fieldname);
    for (i=0; i<comp->argc; i++) {
	if (i) strcatn(buf, size, ", ");
    	strcatn(buf, size, comp->argv[i]);
    }
    strcatn(buf, size, ")");

  } else if (comp->op == OR || comp->op == AND) {
    buf[0] = '\0';

    for (c = comp->group; c; c=c->next) {
      assert(c);
      b = print_comparison(c);
      size += strlen(b) + 5;
      buf = realloc(buf, size);
      strcatn(buf, size, b);
      free(b);

      if (c->next) {
	if (comp->op == AND) {
		strcatn(buf, size, " AND ");
	} else {
		strcatn(buf, size, " OR ");
	}
      }
    }
  } else {
    snprintf(buf, size, "(%s %s \"%s\")", comp->fieldname, opstr(comp), comp->valstr);
  }

  return(buf);
}

static inline struct arglist * arglist_append2(struct arglist * old, struct arglist * append) {
	if (!old) return append;
	arglist_append(old, append);
	return old;
}

struct graph optimize_bools(dts_comparison * comp) {
  dts_comparison *c;
  struct arglist * arglist = NULL;
  struct graph g;

  g.head = NULL; g.tail = NULL;
 
  for(c=comp; c; c = c->next) {
    if (c->op == AND) {
      assert (! "untested");
      graph_join(&g, optimize_bools(c->group));
    } else if (c->op == FUNC) {
      graph_join(&g, newmodule(c->fieldname, c->arglist));
    } else if (c->op == OR) {
      dts_comparison * p;

      for (p=c->group; p; p=p->next) {
	if (p->op != FUNC) {
		arglist = arglist_append2(arglist, newarg("where", 0, NULL));
	}
      	arglist = arglist_append2(arglist, newarg(print_comparison(p), 0, NULL));
      	arglist = arglist_append2(arglist, newarg(";", 0, NULL));
      }

      graph_join(&g, newmodule("lor", arglist)); 
    } else {
      arglist = newarg(print_comparison(c), 0, NULL);
      graph_join(&g, newmodule("filter", arglist));
    }
  }

  return g;
}

dts_comparison * comp_join(dts_comparison * lh, dts_comparison * rh, int isor) {
  dts_comparison * ret = NULL;

  if (!lh) return rh;
  if (!rh) return lh;

  if (!isor) {
   	ret = lh;
	for (; lh->next; lh=lh->next) ;

	lh->next = rh;
  } else if (isor) {
        dts_comparison * left, * right;
        if (lh->next) {
		left = calloc(1,sizeof(dts_comparison));
		left->op = AND;
		left->group = lh;
	} else {
		left = lh;
	}
	if (rh->next) {
		right = calloc(1,sizeof(dts_comparison));
		right->op = AND;
		right->group = rh;
	} else {
		right = rh;
	}

        ret = calloc(1,sizeof(dts_comparison));
	ret->op = OR;
	ret->group = left;
	left->next = right;
  } else {
  	assert(!"Shouldn't get here!");
  }
  	
  return ret;
}

dts_comparison * comp_new(char * field, dts_compare_operation op, char ** argv, int argc) {
     dts_comparison * comp = calloc(1,sizeof(dts_comparison));

     comp->op = op;
     comp->fieldname = field;

     if (comp->op == FUNC) {
       comp->argv = argv;
       comp->argc = argc;
       comp->valstr = field;
     } else {
       comp->field = parse_tenv->requirefield(parse_tenv, field);
       assert(argc <= 1);
       if (argc) comp->valstr = argv[0];
     }

     return comp;
}

