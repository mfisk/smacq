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
	argstr[0] = '\0';

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

	if (newg.head->next_graph) {
		/* This graph has multiple heads that should all become children */
		smacq_graph * sg = newg.head->next_graph;
		smacq_graph * prev = newg.head;

		for( ; sg; prev=sg, sg=sg->next_graph) {
			prev->next_graph = NULL;
			smacq_add_child(graph->tail, sg); 
		}
	}
	
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
	/* Check for function arguments */
	if (ap->isfunc) 
		graph_join(&graph, newmodule(ap->arg, ap->func_args));

                   /* Check for rename options on arguments */
     	if (ap->rename) {
		rename_argc += 2;
		rename_argv = realloc(rename_argv, rename_argc * sizeof(char*));
		rename_argv[rename_argc - 2] = ap->arg;
		rename_argv[rename_argc - 1] = ap->rename;
		ap->arg = ap->rename;
	}

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

struct arglist * newarg(char * arg, enum argtype argtype, struct arglist * func_args) {
     struct arglist * al;
     if (!arg) return NULL;
     
     al = calloc(1, sizeof(struct arglist));
     al->arg = arg;
     if (argtype == FUNCTION) {
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

  case NOT:
    return "NOT";

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

/* Caller must free */
char * print_operand(struct dts_operand * op) {
  char * buf, * op1, * op2;
  int size;

  switch(op->type) {
  	case FIELD:
	  return strdup(op->origin.literal.str);
	  break;

  	case CONST:
	  buf = malloc(3+strlen(op->origin.literal.str));
	  sprintf(buf, "\"%s\"", op->origin.literal.str);
	  return buf;
	  break;

	case ARITH:
	  op1 = print_operand(op->origin.arith.op1);
	  op2 = print_operand(op->origin.arith.op2);
	  size = strlen(op1) + strlen(op2) + 10;
	  buf = malloc(size);
	  snprintf(buf, size, "(%s %s %s)", op1, "*", op2);
	  free(op1); free(op2);
	  return buf;
	  break;
  }
}

static char * qstrcatn(char * dst, int bufsize, const char * src) {
	int left = strlen(dst);
	dst += left;

	left = left - bufsize - 1;
	while (left && (src[0] != '\0')) {
		if (src[0] == '\"') {
			*dst++ = '\\';
			left--;
			if (!left) break;
		}
		*dst++ = *src++;
		left--;
	}

	dst[0] = '\0';
}

char * print_comparison(dts_comparison * comp) {
  int size = 30;
  char * buf = NULL;
  dts_comparison * c;
  char * b;
  char * op1, * op2;
  int i;

  if (!comp) return NULL;

  switch(comp->op) {
	case FUNC:
	  	for (i=0; i<comp->func.argc; i++)
    			size += 2*strlen(comp->func.argv[i])+4;
  		buf = malloc(size);
    		snprintf(buf, size, "%s(", comp->func.name);
    		for (i=0; i<comp->func.argc; i++) {
			if (i) strcatn(buf, size, ",");
			strcatn(buf, size, "\"");
    			qstrcatn(buf, size, comp->func.argv[i]);
			strcatn(buf, size, "\"");
    		}
    		strcatn(buf, size, ")");
		break;

	case EXIST:
		size += strlen(comp->op1->origin.literal.str);
		op1 = print_operand(comp->op1);
  		buf = malloc(size);
    		snprintf(buf, size, "(%s)", print_operand(comp->op1));
		free(op1);
		break;

  	case NOT:
	  assert(comp->group);
	  b = print_comparison(comp->group);
	  size = strlen(b) + 10;
	  buf = realloc(buf, size);
	  strcpy(buf, "NOT ( ");
	  strcatn(buf, size, b);
	  free(b);
	  break;
	  
  	case AND:
        case OR:
          buf = malloc(100);
	  buf[0] = '\0';

	  strcatn(buf, size, "( ");
    			for (c = comp->group; c; c=c->next) {
      			assert(c);
      			b = print_comparison(c);
      			size += strlen(b) + 6;
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
		strcatn(buf, size, " )");
		break;

	default:
	  	op1 = print_operand(comp->op1);
		op2 = print_operand(comp->op2);
		assert(op1); assert(op2);
	  	size += 20 + strlen(op1) + strlen(op2);
  		buf = malloc(size);
    		snprintf(buf, size, "(%s %s %s)", op1, opstr(comp), op2);
		free(op1);
		free(op2);
		break;

  }


  //fprintf(stderr, "parse_comparison output: %s\n", buf);
  return(buf);
}

static inline struct arglist * arglist_append2(struct arglist * old, struct arglist * append) {
	if (!old) return append;
	arglist_append(old, append);
	return old;
}

struct graph optimize_bools(dts_comparison * c) {
  struct arglist * arglist = NULL;
  struct graph g;

  g.head = NULL; g.tail = NULL;

  if (c) {
    if (c->op == AND) {
      dts_comparison * p;

      for (p=c->group; p; p=p->next) {
	graph_join(&g, optimize_bools(p));
      }
    } else if (c->op == FUNC) {
      graph_join(&g, newmodule(c->func.name, c->func.arglist));
    } else if (c->op == OR) {
      struct graph subg;
      dts_comparison * p;
      struct graph uniq;

      subg.head = NULL, subg.tail = NULL;

      arglist = arglist_append2(arglist, newarg("-o", 0, NULL));
      uniq = newmodule("uniq", arglist); 

      for (p=c->group; p; p=p->next) {
        struct graph newg = optimize_bools(p);

	if (!subg.head) {
		subg.head = newg.head;
	} else {
		smacq_graph_add_graph(subg.head, newg.head);
		//fprintf(stderr, "%p(%s) is OR sibling to %p(%s)\n", subg.head, subg.head->name, newg.head, newg.head->name);
	}

	graph_join(&newg, uniq);
      }

      /* Subg encompasses everything we just assembled */
      subg.tail = uniq.tail;
      graph_join(&g, subg);

    } else if (c->op == NOT) {
      arglist = arglist_append2(arglist, newarg("not", 0, NULL));
      if (c->group && c->group->op != FUNC) {
	arglist = arglist_append2(arglist, newarg("where", 0, NULL));
      }
      arglist = arglist_append2(arglist, newarg(print_comparison(c->group), 0, NULL));
				
      graph_join(&g, newmodule("lor", arglist));
    } else {
      arglist = newarg(print_comparison(c), 0, NULL);
      graph_join(&g, newmodule("filter", arglist));
    }
  }

  return g;
}

static dts_comparison * comp_join_andor(dts_comparison * lh, dts_comparison * rh, dts_compare_operation opcode) {
  dts_comparison * ret = NULL;

    if (!lh) return rh;
    if (!rh) return lh;

    if (lh->op == opcode && rh->op == opcode) {
      ret = lh;
      for (lh=lh->group; lh->next; lh=lh->next) ;
      lh->next = rh->group;
      /* rh is now abandoned and can be freed */

    } else if (lh->op == opcode && !rh->next) {
      ret = lh;
      for (lh=lh->group; lh->next; lh=lh->next) ;
      lh->next = rh;

    } else if (rh->op == opcode && !lh->next) {
      ret = rh;
      lh->next = rh->group;
      rh->group = lh;

    } else {
      assert(!lh->next);
      assert(!rh->next);

      ret = calloc(1,sizeof(dts_comparison));
      ret->op = opcode;
      ret->group = lh;
      lh->next = rh;
    }

    return ret;
}
 
dts_comparison * comp_join(dts_comparison * lh, dts_comparison * rh, dts_compare_operation opcode) {
  dts_comparison * ret = NULL;

  if (opcode == AND || opcode == OR) {
    return comp_join_andor(lh, rh, opcode);

  } else if (opcode == NOT) {
    assert(!rh);
    assert(lh);
    ret = calloc(1,sizeof(dts_comparison));
    ret->op = NOT;
    ret->group = lh;
    ret->next = NULL;

  } else {
    assert(!"Shouldn't get here!");
  }
  	
  return ret;
}


struct dts_operand * comp_operand(enum dts_operand_type type, char * str) {
     struct dts_operand * comp = calloc(1,sizeof(struct dts_operand));

     comp->type = type;
     comp->origin.literal.str = str;

     switch (type) {
	case FIELD:
       		comp->origin.literal.field = parse_tenv->requirefield(parse_tenv, str);
		break;

	case CONST:
	       	break;
     }

     return comp;
}

static double zero = 0;

static void make_fields_doubles(dts_environment * tenv, struct dts_operand * operand) {
     assert(operand);
     if (operand->type == FIELD) {
	     char * old = operand->origin.literal.str;
	     operand->origin.literal.str = dts_fieldname_append(old, "double");
	     //free(old);

	     dts_field_free(operand->origin.literal.field);
	     operand->origin.literal.field = dts_requirefield(tenv, operand->origin.literal.str);
     }
}

struct dts_operand * comp_arith(dts_environment * tenv, enum dts_arith_operand_type op, struct dts_operand * op1, struct dts_operand * op2) {
     struct dts_operand * comp = calloc(1,sizeof(struct dts_operand));
   
     comp->type = ARITH;
     comp->origin.arith.type = op;
     comp->origin.arith.op1 = op1;
     comp->origin.arith.op2 = op2;

     /* The arithmetic evaluator needs the following three things: */
     if (!tenv->double_field)
     	tenv->double_field = dts_requirefield(tenv, "double");
     tenv->double_type = dts_requiretype(tenv, "double");
     comp->valueo = dts_construct(tenv, tenv->double_type, &zero);

     make_fields_doubles(tenv, op1);
     make_fields_doubles(tenv, op2);

     return comp;
}

dts_comparison * comp_new(dts_compare_operation op, struct dts_operand * op1, struct dts_operand * op2) {
     dts_comparison * comp = calloc(1,sizeof(dts_comparison));

     comp->op = op;
     comp->op1 = op1;
     comp->op2 = op2;

     return comp;
}

dts_comparison * comp_new_func(char * str, int argc, char ** argv, struct arglist * arglist) {
     dts_comparison * comp = comp_new(FUNC, NULL, NULL);

     comp->func.name = str;
     comp->func.argc = argc;
     comp->func.argv = argv;
     comp->func.arglist = arglist;

     return comp;
}

char * expression2fieldname(struct dts_operand * expr) {
    int i;
    int size = 10;
    char * expr_str;
    
    char * operand = print_operand(expr);
    size += strlen(operand);
    
    expr_str = malloc(size);
    strcpy(expr_str, "_expr_");
    strcat(expr_str, operand);

    { /* Change . to : in fieldname */
      char * i;
      while (i = index(expr_str, '.')) {
	i[0] = ':';
      }
    }

    free(operand);

    return expr_str;
}

