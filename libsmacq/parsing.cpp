#include <smacq-parser.h>
#include <smacq-internal.h>
#include <SmacqGraph.h>
#include <dts.h>

#define tmalloc(type, number) (type*)malloc(number*sizeof(type))

/* Some glue to deal with a plain-C, non-thread-safe parser: */

DTS * parse_dts;
extern dts_comparison * Comp;
extern struct dts_operand * Expr;
extern SmacqGraph * Graph;
extern int yyfilterparse();
extern int yyexprparse();
extern int yysmacql_parse();
extern struct yy_buffer_state * yysmacql_scan_string(const char *);

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
	int i = 0;
	char ** argv;
	struct arglist * al;

	/* Count length of list */
	for(al=alist; al; al=al->next) {
		i++;
	}

	argv = tmalloc(char *, i);
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

	argstr = tmalloc(char, len);
	argstr[0] = '\0';

	for(al=alist; al; al=al->next) {
		strcatn(argstr, len, al->arg);
		if (al->isfunc) {
			char * fargs = arglist2str(al->func_args);
			len += strlen(fargs) + 2;
			argstr = (char*)realloc(argstr, len);
			strcatn(argstr, len, "(");
			strcatn(argstr, len, fargs);
			strcatn(argstr, len, ")");
		}
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
  
  graph->head->join(newg.head);

  graph->tail = newg.tail;
}

void graph_append(struct graph * graph, SmacqGraph * newmod) {
  if (graph->tail) 
    graph->tail->add_child(newmod); 

  graph->tail = newmod;

  if (! graph->head) 
    graph->head = newmod;
}

struct arglist * arglist_append(struct arglist * tail, struct arglist * addition) {
	struct arglist * start;

	if (tail) {
		start = tail;
	} else {
		start = addition;
		addition = NULL;
	}

	if (!start) {
		return NULL;
	}

	for (tail=start; tail->next; tail=tail->next) ;

	tail->next = addition;

	return start;
}

struct graph newgroup(struct group group, struct graph vphrase) {
  /*
   * This function violates some abstractions by knowing the 
   * calling syntax for "groupby" and constructing arguments for it.
   */
  struct graph g = { NULL, NULL};
  char gp[32];
  struct arglist * arglist;
  
  if (!group.args) { 
    /* Do nothing if this was "group by" NULL */
    return vphrase;
  }
  
  if (group.having) { 
    struct graph having = optimize_bools(group.having);
    graph_join(&having, vphrase);
    vphrase = having;
  }

  sprintf(gp, "%lu", (unsigned long)vphrase.head);
  /* smacq_graph_print(stderr, vphrase.head, 0); */

  arglist = newarg("-p", (argtype)0, NULL);
  arglist_append(arglist, newarg(strdup(gp), (argtype)0, NULL));
  arglist_append(arglist, group.args);
 
  g = newmodule("groupby", arglist);
 
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

     anew = newarg(module, (argtype)0, NULL);
     arglist_append(anew, alist);

     for(ap=anew; ap; ap=ap->next) {
	/* Check for function arguments */
	if (ap->isfunc) 
		graph_join(&graph, newmodule(ap->arg, ap->func_args));

                   /* Check for rename options on arguments */
     	if (ap->rename) {
		rename_argc += 2;
		rename_argv = (char**)realloc(rename_argv, rename_argc * sizeof(char*));
		rename_argv[rename_argc - 2] = ap->arg;
		rename_argv[rename_argc - 1] = ap->rename;
		ap->arg = ap->rename;
	}

     }

     if (rename_argc > 1) {
        /* We need to splice in a rename module before this module */
     	rename_argv[0] = "rename";
        graph_append(&graph, new SmacqGraph(rename_argc, rename_argv));
     }

     arglist2argv(anew, &argc, &argv);
     graph_append(&graph, new SmacqGraph(argc, argv));

     return graph;
}

struct arglist * newarg(char * arg, enum argtype argtype, struct arglist * func_args) {
     struct arglist * al;
     if (!arg) return NULL;
     
     al = (struct arglist*)calloc(1, sizeof(struct arglist));
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
	  buf = tmalloc(char, 3+strlen(op->origin.literal.str));
	  sprintf(buf, "\"%s\"", op->origin.literal.str);
	  return buf;
	  break;

	case ARITH:
	  op1 = print_operand(op->origin.arith.op1);
	  op2 = print_operand(op->origin.arith.op2);
	  size = strlen(op1) + strlen(op2) + 10;
	  buf = tmalloc(char, size);
	  snprintf(buf, size, "(%s %s %s)", op1, "*", op2);
	  free(op1); free(op2);
	  return buf;
	  break;
  }

  return NULL;
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

	return dst;
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
    buf = tmalloc(char, size);
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
    buf = tmalloc(char, size);
    snprintf(buf, size, "(%s)", print_operand(comp->op1));
    free(op1);
    break;
    
  case NOT:
    assert(comp->group);
    b = print_comparison(comp->group);
    size = strlen(b) + 10;
    buf = (char*)realloc(buf, size);
    strcpy(buf, "NOT ( ");
    strcatn(buf, size, b);
    free(b);
    break;
    
  case AND:
  case OR:
    buf = tmalloc(char, 100);
    buf[0] = '\0';
    
    strcatn(buf, size, "( ");
    for (c = comp->group; c; c=c->next) {
      assert(c);
      b = print_comparison(c);
      size += strlen(b) + 6;
      buf = (char*)realloc(buf, size);
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
    buf = tmalloc(char, size);
    snprintf(buf, size, "(%s %s %s)", op1, opstr(comp), op2);
    free(op1);
    free(op2);
    break;
    
  }


  //fprintf(stderr, "parse_comparison output: %s\n", buf);
  return(buf);
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

      arglist = arglist_append(arglist, newarg("-o", (argtype)0, NULL));
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
      arglist = arglist_append(arglist, newarg("not", (argtype)0, NULL));
      if (c->group && c->group->op != FUNC) {
	arglist = arglist_append(arglist, newarg("where", (argtype)0, NULL));
      }
      arglist = arglist_append(arglist, newarg(print_comparison(c->group), (argtype)0, NULL));
				
      graph_join(&g, newmodule("lor", arglist));

    } else if (c->op == EQ && c->op1->type == FIELD && c->op2->type == CONST) {

      arglist = newarg(c->op1->origin.literal.str, (argtype)0, NULL);
      arglist = arglist_append(arglist, newarg(c->op2->origin.literal.str, (argtype)0, NULL));
      graph_join(&g, newmodule("equals", arglist));

    } else {
      arglist = newarg(print_comparison(c), (argtype)0, NULL);
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

      ret = (dts_comparison*)calloc(1,sizeof(dts_comparison));
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
    ret = (dts_comparison*)calloc(1,sizeof(dts_comparison));
    ret->op = NOT;
    ret->group = lh;
    ret->next = NULL;

  } else {
    assert(!"Shouldn't get here!");
  }
  	
  return ret;
}


struct dts_operand * comp_operand(enum dts_operand_type type, char * str) {
     struct dts_operand * comp = (struct dts_operand*)calloc(1,sizeof(struct dts_operand));

     comp->type = type;
     comp->origin.literal.str = str;

     switch (type) {
	case FIELD:
	  comp->origin.literal.field = parse_dts->requirefield(str);
	  break;

	case CONST:
	  break;

                   case ARITH:
		     assert(0);
		     break;
     }

     return comp;
}

static double zero = 0;

void DTS::make_fields_doubles(struct dts_operand * operand) {
     assert(operand);
     if (operand->type == FIELD) {
	     char * old = operand->origin.literal.str;
	     operand->origin.literal.str = dts_fieldname_append(old, "double");
	     //free(old);

	     dts_field_free(operand->origin.literal.field);
	     operand->origin.literal.field = requirefield(operand->origin.literal.str);
     }
}

		
struct dts_operand * DTS::comp_arith(enum dts_arith_operand_type op, struct dts_operand * op1, struct dts_operand * op2) {
     struct dts_operand * comp = (struct dts_operand *)calloc(1,sizeof(struct dts_operand));
   
     comp->type = ARITH;
     comp->origin.arith.type = op;
     comp->origin.arith.op1 = op1;
     comp->origin.arith.op2 = op2;
     comp->valueo = construct(requiretype("double"), &zero);

     make_fields_doubles(op1);
     make_fields_doubles(op2);

     return comp;
}

dts_comparison * comp_new(dts_compare_operation op, struct dts_operand * op1, struct dts_operand * op2) {
     dts_comparison * comp = (dts_comparison*)calloc(1,sizeof(dts_comparison));

     if (op != FUNC && op != NOT && op != EXIST && op1->type != FIELD && op2->type == FIELD) {
       comp->op2 = op1;
       comp->op1 = op2;
       switch (op) {
       case EQ:
       case NEQ:
       case AND:
       case OR:
       case LIKE:
	 comp->op = op;
	 break;
       case GT:
	 comp->op = LEQ;
	 break;
       case GEQ:
	 comp->op = LT;
	 break;
       case LT:
	 comp->op = GEQ;
	 break;
       case LEQ:
	 comp->op = GT;
	 break;
       case NOT:
       case FUNC:
       case EXIST:
	 assert(0);
	 break;
       }
     } else {
       comp->op = op;
       comp->op1 = op1;
       comp->op2 = op2;
     }

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
    int size = 10;
    char * expr_str;
    
    char * operand = print_operand(expr);
    size += strlen(operand);
    
    expr_str = tmalloc(char, size);
    strcpy(expr_str, "_expr_");
    strcat(expr_str, operand);

    { /* Change . to : in fieldname */
      char * i;
      while ((i = index(expr_str, '.'))) {
	i[0] = ':';
      }
    }

    free(operand);

    return expr_str;
}



dts_comparison * DTS::parse_tests(int argc, char ** argv) {
  dts_comparison * retval;
  int size = 1;
  int i;
  char * qstr;

  /* LOCK */
  parse_dts = this;

  for (i=0; i<argc; i++) {
  	size += strlen(argv[i]);
  }
  size += argc;

  qstr = (char*)malloc(size);
  qstr[0] = '\0';
  	
  for (i=0; i<argc; i++) {
  	strcatn(qstr, size, argv[i]);
  	strcatn(qstr, size, " ");
  }
  yysmacql_scan_string(qstr);
  //fprintf(stderr, "parsing filter buffer: %s\n", qstr); 

  if (yyfilterparse()) {
  	/* Should free the comparisons? */
        fprintf(stderr, "got nonzero return parsing boolean %s\n", qstr); 
  	return NULL;
  }

  retval = Comp;

  /* UNLOCK */
  print_comp(retval);

  return retval;
}

void DTS::print_comp(dts_comparison * c) {
	char * op;
	op = "UNDEFINED";

	if (!c) {
		// fprintf(stderr,". (%p)\n", c);
		return;
	}

	switch(c->op) {
		case AND: op = "AND"; break;
		case OR: op = "OR"; break;
		case GT: op = "<"; break;
		case LT: op = ">"; break;
		case EQ: op = "="; break;
		case NEQ: op = "!="; break;
		case GEQ: op = ">="; break;
		case LEQ: op = "<="; break;
		case EXIST: op = "exist"; break;
		case LIKE: op = "like"; break;
		case FUNC: op = "FN"; break;
		case NOT: op = "!"; break;
	}

#if DEBUG
	switch(c->op) {
		case AND:
		case OR:
			fprintf(stderr, "Comparison %p: op %s, next %p, group %p\n", c, op, c->next, c->group);
			break;

		case FUNC:
			fprintf(stderr, "Comparison %p: FN %s(), next %p, group %p\n", c, c->valstr, c->next, c->group);
			break;

		default:
			fprintf(stderr, "Comparison %p: field %d... %s %s, next %p, group %p\n", c, c->field[0], op, c->valstr, c->next, c->group);
			break;
			
	}
#endif

	print_comp(c->group);
	print_comp(c->next);

}


struct dts_operand * DTS::parse_expr(int argc, char ** argv) {
  struct dts_operand * retval;
  int size = 1;
  int i;
  char * qstr;

  /* LOCK */
  parse_dts = this;

  for (i=0; i<argc; i++) {
  	size += strlen(argv[i]);
  }
  size += argc;

  qstr = (char*)malloc(size);
  qstr[0] = '\0';
  	
  for (i=0; i<argc; i++) {
  	strcatn(qstr, size, argv[i]);
  	strcatn(qstr, size, " ");
  }
  yysmacql_scan_string(qstr);
  //fprintf(stderr, "parsing expr buffer: %s\n", qstr); 

  if (yyexprparse()) {
        fprintf(stderr, "got nonzero return parsing expression %s\n", qstr); 
  	return NULL;
  }

  retval = Expr;

  /* UNLOCK */

  return retval;
}

/// Parse a query and return the SmacqGraph that executes it.
SmacqGraph * SmacqGraph::newQuery(DTS * tenv, int argc, char ** argv) {
  int size = 0;
  int i;
  char * qstr; 
  SmacqGraph * graph;
  int res;

  parse_dts = tenv;

  for (i=0; i<argc; i++) {
  	size += strlen(argv[i]);
  }
  size += argc;

  qstr = (char*)malloc(size);
  qstr[0] = '\0';
  	
  for (i=0; i<argc; i++) {
  	strcatn(qstr, size, argv[i]);
  	strcatn(qstr, size, " ");
  }

  /* LOCK */
  smacq_pthread_mutex_lock(&local_lock);

  yysmacql_scan_string(qstr);
  //fprintf(stderr, "parsing buffer: %s\n", qstr); 

  res = yysmacql_parse();

  graph = Graph;

  /* UNLOCK */
 smacq_pthread_mutex_unlock(&local_lock);

  if (res) {
    fprintf(stderr, "SmacqGraph::newQuery: error parsing query: %s\n", qstr);
    return NULL;
  }

  if (!graph) {
    fprintf(stderr, "unknown parse error\n");
  }

  return graph;
}
