#include <smacq-parser.h>
#include <smacq-internal.h>
#include <SmacqGraph.h>
#include <dts.h>

#define tmalloc(type, number) (type*)malloc(number*sizeof(type))

/* Some glue to deal with a plain-C, non-thread-safe parser: */

DTS * parse_dts;
SmacqScheduler * parse_sched;

extern dts_comparison * Comp;
extern struct dts_operand * Expr;
extern SmacqGraph * Graph;
extern int yyfilterparse();
extern int yyexprparse();
extern int yysmacql_parse();
extern struct yy_buffer_state * yysmacql_scan_string(const char *);
char * ParsedString;

void graph_join(SmacqGraph ** oldg, SmacqGraph * newg) {
  assert(oldg);
  if (*oldg) {
    (*oldg)->join(newg);
  } else {
    *oldg = newg;
  }
}

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

SmacqGraph * newgroup(struct group group, SmacqGraph * vphrase) {
  /*
   * This function violates some abstractions by knowing the 
   * calling syntax for "groupby" and constructing arguments for it.
   */
  SmacqGraph * g = NULL;
  char gp[32];
  struct arglist * arglist;
  
  if (!group.args) { 
    /* Do nothing if this was "group by" NULL */
    return vphrase;
  }
  
  if (group.having) { 
    SmacqGraph * having = optimize_bools(group.having);
    graph_join(&having, vphrase);
    vphrase = having;
  }

  sprintf(gp, "%p", vphrase);

  arglist = newarg("-p", (argtype)0, NULL);
  arglist_append(arglist, newarg(strdup(gp), (argtype)0, NULL));
  arglist_append(arglist, group.args);
 
  g = newmodule("groupby", arglist);
 
  return g;
}

SmacqGraph * newmodule(char * module, struct arglist * alist) {
  struct arglist * anew;
  SmacqGraph * graph = NULL;

  int argc;
  char ** argv;

  int rename_argc = 1;
  char ** rename_argv = NULL;
  struct arglist * ap;
  int num_funcs = 0;
  SmacqGraph * uniq_obj = NULL;

  anew = newarg(module, (argtype)0, NULL);
  arglist_append(anew, alist);

  // Count number of func arguments
  for(ap=anew; ap; ap=ap->next) {
	if (ap->isfunc) {
		num_funcs++;
	}
  }

  if (num_funcs > 1) {
	char ** argv = (char**)malloc(sizeof(char**));
	argv[0] = "uniqobj";
	uniq_obj = new SmacqGraph(1, argv);
  }

  for(ap=anew; ap; ap=ap->next) {
    /* Check for function arguments */
    if (ap->isfunc) { 
      struct arglist * arglist;
      char * argname = ap->arg;

      if (ap->rename) {
  	arglist = newarg("-f", (argtype)0, NULL);
  	arglist_append(arglist, newarg(ap->rename, (argtype)0, NULL));
  	arglist_append(arglist, ap->func_args);
	ap->arg = ap->rename;
      } else {
	arglist = ap->func_args;
      }

      SmacqGraph * fn = newmodule(argname, arglist);

      if (num_funcs > 1) {
        fn->add_child(uniq_obj);
      } 
      if (graph) {
	graph->add_graph(fn); 
      } else {
	graph = fn;
      }
    } else {
    	/* Check for rename options on arguments */
    	if (ap->rename) {
      		rename_argc += 2;
      		rename_argv = (char**)realloc(rename_argv, rename_argc * sizeof(char*));
      		rename_argv[rename_argc - 2] = ap->arg;
      		rename_argv[rename_argc - 1] = ap->rename;
      		ap->arg = ap->rename;
	}
    }

  }

  if (rename_argc > 1) {
    /* We need to splice in a rename module before this module */
    rename_argv[0] = "rename";
    graph_join(&graph, new SmacqGraph(rename_argc, rename_argv));
  }

  arglist2argv(anew, &argc, &argv);
  graph_join(&graph, new SmacqGraph(argc, argv));

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

  case EXIST:
    return "";

  case AND:
    return "AND";

  case OR:
    return "OR";
 
  case FUNC:
    assert(0);
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
    delete(op1); delete(op2);
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
    delete(op1);
    break;
    
  case NOT:
    assert(comp->group);
    b = print_comparison(comp->group);
    size = strlen(b) + 10;
    buf = (char*)realloc(buf, size);
    strcpy(buf, "NOT ( ");
    strcatn(buf, size, b);
    delete(b);
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
      delete(b);
      
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
    delete(op1);
    delete(op2);
    break;
    
  }


  //fprintf(stderr, "parse_comparison output: %s\n", buf);
  return(buf);
}

SmacqGraph * optimize_bools(dts_comparison * c) {
  struct arglist * arglist = NULL;
  SmacqGraph * g = NULL;

  if (c) {
    if (c->op == AND) {
      dts_comparison * p;

      for (p=c->group; p; p=p->next) {
	graph_join(&g, optimize_bools(p));
      }
    } else if (c->op == FUNC) {
      graph_join(&g, newmodule(c->func.name, c->func.arglist));
    } else if (c->op == OR) {
      SmacqGraph * subg = NULL;
      dts_comparison * p;
      SmacqGraph * uniq;

      uniq = newmodule("uniqobj", NULL); 

      for (p=c->group; p; p=p->next) {
        SmacqGraph * newg = optimize_bools(p);

	if (!subg) {
	  subg = newg;
	} else {
	  smacq_graph_add_graph(subg, newg);
	  //fprintf(stderr, "%p(%s) is OR sibling to %p(%s)\n", subg, subg->name, newg, newg->name);
	}

	graph_join(&newg, uniq);
      }

      /* Subg encompasses everything we just assembled */
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

    ret = new dts_comparison;
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
    ret = new dts_comparison;
    ret->op = NOT;
    ret->group = lh;
    ret->next = NULL;

  } else {
    assert(!"Shouldn't get here!");
  }
  	
  return ret;
}


struct dts_operand * comp_operand(enum dts_operand_type type, char * str) {
  dts_operand * comp = new dts_operand;

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
    //delete(old);
 
    operand->origin.literal.field = requirefield(operand->origin.literal.str);
  }
}

		
struct dts_operand * DTS::comp_arith(enum dts_arith_operand_type op, struct dts_operand * op1, struct dts_operand * op2) {
  dts_operand * comp = new dts_operand;
   
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
  dts_comparison * comp = new dts_comparison;

  if (op != FUNC && op != NOT && op != EXIST && op1->type != FIELD && op2->type == FIELD) {
    comp->op2 = op1;
    comp->op1 = op2;
    switch (op) {
    case EQ:
    case NEQ:
    case AND:
    case OR:
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

  delete(operand);

  return expr_str;
}

void SmacqModule::comp_uses(dts_comparison * c) {
  if (c->op != AND && c->op != OR && c->op != NOT && c->op != FUNC) {
    if (c->op1->type == FIELD) 
      usesFields[c->op1->origin.literal.field[0]] = true;

    if (c->op != EXIST && c->op2->type == FIELD) 
      usesFields[c->op2->origin.literal.field[0]] = true;
  }
}

dts_comparison * SmacqModule::parse_tests(int argc, char ** argv) {
  dts_comparison * retval;

  /* XXX LOCK */

  parse_dts = dts;
  char * qstr = argv2str(argc, argv);

  yysmacql_scan_string(qstr);
  //fprintf(stderr, "parsing filter buffer: %s\n", qstr); 

  char * oldParsedString = ParsedString;
  ParsedString = qstr;
  int res = yyfilterparse();
  ParsedString = oldParsedString;

  if (res) {
    /* Should free the comparisons? */
    //fprintf(stderr, "got nonzero return parsing boolean %s\n", qstr); 
    return NULL;
  }

  retval = Comp;
  comp_uses(Comp);

  /* UNLOCK */

  return retval;
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

  char * oldParsedString = ParsedString;
  ParsedString = qstr;
  int res = yyexprparse();
  ParsedString = oldParsedString;

  if (res) {
    return NULL;
  }

  retval = Expr;

  /* UNLOCK */

  return retval;
}

/// Parse a query and return the SmacqGraph that executes it.
SmacqGraph * SmacqGraph::newQuery(DTS * tenv, SmacqScheduler * sched, int argc, char ** argv) {
  int size = 0;
  int i;
  char * qstr; 
  SmacqGraph * graph;
  int res;

  parse_dts = tenv;
  parse_sched = sched;

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

  char * oldParsedString = ParsedString;
  ParsedString = qstr;
  yysmacql_scan_string(qstr);
  res = yysmacql_parse();
  ParsedString = oldParsedString;

  graph = Graph;

  /* UNLOCK */
  smacq_pthread_mutex_unlock(&local_lock);

  if (res) {
    return NULL;
  }

  if (!graph) {
    fprintf(stderr, "unknown parse error\n");
  }

  return graph;
}

SmacqGraph * joinlist2graph(joinlist * joinlist, SmacqGraph * where) {
  /*
   * This function violates some abstractions by knowing the 
   * calling syntax for "join" and constructing arguments for it.
   */
  SmacqGraph * g = NULL;
  char gp[32];
  struct arglist * arglist = NULL;
  SmacqGraph * aliases = NULL;
  SmacqGraph * thisgraph = NULL;

  while (joinlist) {
    if (joinlist->graph) thisgraph = joinlist->graph;

    SmacqGraph * newalias = newmodule("streamalias", 
				      newarg(joinlist->name, (argtype)0, NULL));

    DtsField thisfield = parse_dts->requirefield(joinlist->name);
    if (where) {
      SmacqGraph * invariants = where->get_invariants_over_field(parse_dts, parse_sched, thisfield);
      //assert(invariants);
      if (invariants) newalias->add_child(invariants);
    }
	
    if (aliases) {
      aliases->add_graph(newalias);
    } else {
      aliases = newalias;
    }

    if (!joinlist->next || joinlist->next->graph) {
      // Last alias for this graph

      assert(thisgraph);
      thisgraph->join(aliases);
      aliases = NULL;

      if (g) {
	g->add_graph(thisgraph);
      } else {
	g = thisgraph;
      }
    }

    arglist = arglist_append(arglist, newarg(joinlist->name, (argtype)0, NULL));

    // Add UNTIL clause pointer
    sprintf(gp, "%p", joinlist->until);
    arglist = arglist_append(arglist, newarg(strdup(gp), (argtype)0, NULL));

    joinlist = joinlist->next;
  }

  // Add WHERE clause pointer
  sprintf(gp, "%p", where);
  arglist = arglist_append(arglist, newarg(strdup(gp), (argtype)0, NULL));
 
  SmacqGraph * jg = newmodule("join", arglist);
  g->join(jg);
 
  return g;
}

void yyerror(char * msg) {
  if (yytext[0] != '\0') {
    fprintf(stderr, "Error: %s near \"%s\" in line: %s\n", msg, yytext, ParsedString);
  } else {
    fprintf(stderr, "Error: Unexpected end of statement: %s\n", ParsedString);
  }
}

