#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// PostgreSQL includes
//
#include <postgres.h>		// for lots of stuff
#include <postgres_ext.h>	// for Oid typedef
#include "parsenodes.h"     // our copy with modified SelectStmt struct
#include <pg_list.h>		// for List typedef
#include <tcopprot.h>		// for pg_parse_and_rewrite() FP
#include <guc.h>			// for config context flag
#include <memutils.h>		// for AllocSetContextCreate() call
#include <stringinfo.h>		// for AllocSetContextCreate() call

#include <parser.h>
#include <print.h>
#include <analyze.h>
#include <rewriteHandler.h>

#include <gram.h>

//-------------------- BEGIN GLOBAL VARS -----------------------------

int yyparse();
void scanner_init();
char * parseString;
List * parsetree;

//int yydebug = 1;

int transformStmt(List *);
int transformSelectStmt(SelectStmt *);

//#undef DEBUG

char * cmd_array[30]; // TBD
int curr_idx;
char * from_array[30]; // TBD
int curr_from_idx;
char * print_array[30]; // TBD
int curr_print_idx;
char * final_array[100]; // TBD
int curr_final_idx;

int func_list_count;

typedef struct _list_item {
  char * str_name;
  struct list * arg_list;
  struct _list_item * prev;
  struct _list_item * next;
} list_item;

typedef struct _list {
  char * list_name;
  int num_items;
  list_item * head;
  list_item * tail;
  struct _list * prev;
  struct _list * next;
} list;

typedef struct _list_list {
  char * list_name;
  int num_items;
  list * head;
  list * tail;
} list_list;

list * print_list;
list_list * func_list_list;

struct annot_func {
  char * name;
  int num;
  char * var_names[5];
} annot_funcs[6] = {
  { "count", 0, { NULL, NULL, NULL, NULL, NULL } },
  { "delta", 0, { NULL, NULL, NULL, NULL, NULL } },
  { "derivative", 0, { NULL, NULL, NULL, NULL, NULL } },
  { "entropy", 0, { NULL, NULL, NULL, NULL, NULL } },
  { "round", 0, { NULL, NULL, NULL, NULL, NULL } },
  { /* EOF */ NULL, 0, { NULL, NULL, NULL, NULL, NULL } } 
}
;

//-------------------- END GLOBAL VARS -------------------------------

bool is_annotation_function(char * fn, int * idx)
{
  int i;
  char * name = annot_funcs[i].name;
  for (i=0; name != NULL; i++) {
	if (strcmp(fn, name) == 0) {
      *idx = i;
	  return TRUE;
    }
  }
  return FALSE;
}

char * create_annotation_variable(int i)
{
  // eg. count1, count2, etc.
  int ivar;
  ivar = annot_funcs[i].num; 
  annot_funcs[i].var_names[ivar] = malloc(strlen(annot_funcs[i].name) + 2);
  sprintf(annot_funcs[i].var_names[ivar], "%s%i", annot_funcs[i].name, ivar+1);
  annot_funcs[i].num++;
#ifdef DEBUG
  fprintf(stderr, "create_annotation_variable(): annot_funcs[%i].var_names[%i] = %s\n", i, ivar, annot_funcs[i].var_names[ivar]);
#endif
  return annot_funcs[i].var_names[ivar];
}

list * make_list(char * name)
{
  list * l = malloc(sizeof(list));
  l->list_name = malloc(strlen(name));
  sprintf(l->list_name, name);
#ifdef DEBUG
  fprintf(stderr, "make_list: Creating new list '%s' at %p\n", l->list_name, l);
#endif
  l->num_items = 0;
  l->head = NULL;
  l->tail = NULL;
  return l;
}

list_list * make_list_list(char * name)
{
  list_list * l = malloc(sizeof(list_list));
  l->list_name = malloc(strlen(name));
  sprintf(l->list_name, name);
#ifdef DEBUG
  fprintf(stderr, "make_list_list: Creating new list_list '%s' at %p\n", l->list_name, l);
#endif
  l->num_items = 0;
  l->head = NULL;
  l->tail = NULL;
  return l;
}

list_item * make_list_item(char * name) 
{
  list_item * li = malloc(sizeof(list_item));
  li->str_name = malloc(strlen(name));
  sprintf(li->str_name, name);
  li->arg_list = NULL;
  return li;
}

void d2_list_enqueue(list * l, list_item * new) 
{
#ifdef DEBUG
  fprintf(stderr, "ENQUEING '%s' w/ addr %p, onto list '%s' w/ addr %p, w/ %d items beforehand.\n", new->str_name, new, l->list_name, l, l->num_items);
#endif

  if (l->num_items == 0) {
	new->prev = new->next = NULL;
    l->head = l->tail = new;
  }
  else {
    l->tail->next = new;
	new->prev = l->tail;
	new->next = NULL;
    l->tail = new;
  }
  l->num_items++;
}

void d2_list_list_enqueue(list_list * l, list * new) 
{
#ifdef DEBUG
  fprintf(stderr, "ENQUEING '%s' w/ addr %p, onto list_list '%s' w/ addr %p, w/ %d items beforehand.\n", new->list_name, new, l->list_name, l, l->num_items);
#endif

  if (l->num_items == 0) {
	new->prev = new->next = NULL;
    l->head = l->tail = new;
  }
  else {
    l->tail->next = new;
	new->prev = l->tail;
	new->next = NULL;
    l->tail = new;
  }
  l->num_items++;
}

enum {
  FROM_HEAD,
  FROM_TAIL
};

list_item * d2_list_dequeue(list * l, int end) 
{
  list_item * curr;

  if (end == FROM_HEAD) {
    if (l->head) {
      curr = l->head;
      l->head = l->head->next;
      l->num_items--;
#ifdef DEBUG
      fprintf(stderr, "DEQUEING HEAD '%s' w/ addr %p from list '%s' w/ addr %p, w/ num_items after = %d\n", curr->str_name, curr, l->list_name, l, l->num_items);
#endif
      return(curr);
    }
    else {
      return (list_item *)NULL;
    }
  } else { // FROM_TAIL
    if (l->tail) {
	  curr = l->tail;
	  l->tail = l->tail->prev;
      l->num_items--;
#ifdef DEBUG
      fprintf(stderr, "DEQUEING TAIL '%s' w/ addr %p from list '%s' w/ addr %p, w/ num_items after = %d\n", curr->str_name, curr, l->list_name, l, l->num_items);
#endif
      return(curr);
	}
    else {
      return (list_item *)NULL;
    }
  }
}

list * d2_list_list_dequeue(list_list * l, int end) 
{
  list * curr;

  if (end == FROM_HEAD) {
    if (l->head) {
      curr = l->head;
      l->head = l->head->next;
      l->num_items--;
#ifdef DEBUG
      fprintf(stderr, "DEQUEING HEAD '%s' w/ addr %p from list_list '%s' w/ addr %p, w/ num_items after = %d\n", curr->list_name, curr, l->list_name, l, l->num_items);
#endif
      return(curr);
    }
    else {
      return (list *)NULL;
    }
  } else { // FROM_TAIL
    if (l->tail) {
	  curr = l->tail;
	  l->tail = l->tail->prev;
      l->num_items--;
#ifdef DEBUG
      fprintf(stderr, "DEQUEING TAIL '%s' w/ addr %p from list_list '%s' w/ addr %p, w/ num_items after = %d\n", curr->list_name, curr, l->list_name, l, l->num_items);
#endif
      return(curr);
	}
    else {
      return (list *)NULL;
    }
  }
}

void dump_array()
{
#ifdef DEBUG
  int i;

  fprintf(stderr, "dump_array:\n");
  for (i = 0; i < curr_idx; i++) {
	fprintf(stderr, "ARRAY[%d] = '%s'\n", i, cmd_array[i]);
  }
#endif
}

void dump_final_array()
{
#ifdef DEBUG
  int i;

  fprintf(stderr, "dump_final_array:\n");
  for (i = 0; i < curr_final_idx; i++) {
	fprintf(stderr, "ARRAY[%d] = '%s'\n", i, final_array[i]);
  }
#endif
}

void append_array(char * s)
{
  cmd_array[curr_idx] = malloc(1 + strlen(s));
  strcpy(cmd_array[curr_idx], s);
#ifdef DEBUG
  fprintf(stderr, "append_array: array[%d] = '%s'\n", curr_idx, cmd_array[curr_idx]);
#endif
  curr_idx++;
}

void append_from_array(char * s)
{
  from_array[curr_from_idx] = malloc(1 + strlen(s));
  strcpy(from_array[curr_from_idx], s);
#ifdef DEBUG
  fprintf(stderr, "append_from_array: array[%d] = '%s'\n", curr_from_idx, from_array[curr_from_idx]);
#endif
  curr_from_idx++;
}

void append_print_array(char * s)
{
  print_array[curr_print_idx] = malloc(1 + strlen(s));
  strcpy(print_array[curr_print_idx], s);
#ifdef DEBUG
  fprintf(stderr, "append_print_array: array[%d] = '%s'\n", curr_print_idx, print_array[curr_print_idx]);
#endif
  curr_print_idx++;
}

void append_final_array(char * s)
{
  final_array[curr_final_idx] = malloc(1 + strlen(s));
  strcpy(final_array[curr_final_idx], s);
#ifdef DEBUG
  fprintf(stderr, "append_final_array: array[%d] = '%s'\n", curr_final_idx, final_array[curr_final_idx]);
#endif
  curr_final_idx++;
}

char * build_string_from_list(list * l) {
  list_item * li;
  char * s = malloc(256);

  for (li = l->head; li; li = li->next) {
	sprintf(s, "%s", li->str_name);
  }
  return s;
}

void build_char_array_from_list(list * l) {
  list_item * li;

  for (li = l->head; li; li = li->next) {
	fprintf(stderr, "build_char_array_from_list: curr_idx = %d\n", curr_idx);
	cmd_array[curr_idx] = malloc(1 + strlen(li->str_name));
	cmd_array[curr_idx] = li->str_name;
	curr_idx++;
  }
}

void free_all()
{
}

enum {
  SELECT_CLAUSE,
  FROM_CLAUSE,
  WHERE_CLAUSE,
  GROUP_CLAUSE,
};

Node * transformExpr(Node *expr, list * func_list, int clause_type)
{
  Node * result = NULL;

  if (expr == NULL)
    return NULL;

#ifdef DEBUG
  fprintf(stderr, "NodeTag in transformExpr = %d\n", nodeTag(expr));
#endif
  switch (nodeTag(expr)) {
    case T_Attr: {
#ifdef DEBUG
      fprintf(stderr, "Found Attr in Expr: \n");
#endif
      break;
    }
    case T_A_Const: {
      A_Const * c = (A_Const *) expr;
      char * str = c->val.val.str; // eg. '128.165.13.27'
#ifdef DEBUG
      fprintf(stderr, "Found A_Const in Expr: %s\n", str);
#endif

      append_array(str);

      break;
    }
    case T_A_Expr: {
      A_Expr * a = (A_Expr *) expr;
	  Node * lexpr, * rexpr;
#ifdef DEBUG
      fprintf(stderr, "a->oper in transformExpr =   %d\n", a->oper);
      fprintf(stderr, "a->opname in transformExpr = %s \n", a->opname);
#endif
      switch (a->oper) {
        case OP: {
#ifdef DEBUG
          fprintf(stderr, "Found A_Expr, OP in Expr: %s \n", a->opname);
#endif
		  lexpr = transformExpr(a->lexpr, func_list, clause_type);

          append_array(a->opname);

		  rexpr = transformExpr(a->rexpr, func_list, clause_type);

		  break;
        }
		case T_Ident: {
#ifdef DEBUG
          Ident * id = (Ident *) expr;
          fprintf(stderr, "Found Ident in A_Expr: %s\n", id->name);
#endif
		  break;
		}
        default: {
		  break;
        }
      }
	  break;
    }
	case T_FuncCall: {
      FuncCall * fc = (FuncCall *) expr;
	  char * funcname = fc->funcname;
      List * al;
	  list_item * li;
	  list_item * ali; // for printf, remove later
	  list * arg_list;

#ifdef DEBUG
      fprintf(stderr,"Found FuncCall in Expr: %s\n", funcname);
#endif

	  li = make_list_item(funcname);

	  if (fc->args) {
		char * lname = malloc(strlen(funcname) + strlen("-arg_list"));
        sprintf(lname, "%s-arg_list", funcname);
		arg_list = make_list(lname);
		(list *)li->arg_list = arg_list;
	  }
	  else {
		arg_list = (list *)NULL;
	  }

	  d2_list_enqueue(func_list, li);

      foreach(al, fc->args) {
        Node * arg = lfirst(al);
#ifdef DEBUG
		fprintf(stderr, "Found node type %d in FuncCall in transformExpr\n", arg->type);
#endif
        if (IsA(arg, Ident)) {
          Ident * id = (Ident *)arg;
		  char * name = id->name;
		  list_item * ali = make_list_item(name);

#ifdef DEBUG
          fprintf(stderr, "Found Ident arg in FuncCall: %s\n", name);
#endif

		  d2_list_enqueue(arg_list, ali);
        }
		else if (IsA(arg, A_Const)) {
		  A_Const * s = (A_Const *)arg;
		  char * str = s->val.val.str;
		  list_item * ali = make_list_item(str);

#ifdef DEBUG
          fprintf(stderr, "Found A_Const arg in FuncCall: %s\n", str);
#endif

		  d2_list_enqueue(arg_list, ali);
		}
		else if (IsA(arg, FuncCall)) {
#ifdef DEBUG
          fprintf(stderr, "Found FuncCall arg in FuncCall:\n");
          fprintf(stderr,"  Calling transformExpr...\n");
#endif

		  transformExpr(arg, func_list, clause_type);
		}
		else {
#ifdef DEBUG
          fprintf(stderr, "Found UNKNOWN arg type in FuncCall in transformExpr:\n");
#endif
		  //break; //TBD
		}
      }

     {
	  // append the innermost function arg to each outer function
	  // eg. count(uniq(dstip)) -> uniq dstip | count dstip
	  //
	  list_item * tail_item = func_list->tail;
	  list * tail_list = (list *)(tail_item->arg_list);
	  list_item * head_item = tail_list->head;

#ifdef DEBUG
	  fprintf(stderr, "INNER ARG = %s\n", head_item->str_name);
#endif

      for (ali = func_list->head; ali != func_list->tail; ali = ali->next) {
#ifdef DEBUG
	    fprintf(stderr, "ARG FOR %s = '%s'\n", li->str_name, ali->str_name);
	    fprintf(stderr, "LI = %p, func_list->tail = %p\n", li, func_list->tail);
#endif
		  d2_list_enqueue((list *)ali->arg_list, head_item);
	  }
	 }

      break;
    }
    case T_Ident: {
      Ident * id = (Ident *) expr;
#ifdef DEBUG
      fprintf(stderr, "Found Ident in Expr: %s\n", id->name);
#endif

      append_array(id->name);

      break;
    }
	default: {
	  break;
	} 
  }

  return result;
}

//
// select target list, eg. SELECT t1, t2, etc.
//
int transformTargetList(List * target_list, list * func_list, bool isPrint)
{
  int rc = 0;
  //Node * result;
  list_item * li;
  list * arg_list;

#ifdef DEBUG
  fprintf(stderr, "Found Target List: \n");
#endif

  if (isPrint) {
    print_list = make_list("print_list");
    append_print_array("|");
    append_print_array("print");
  }
  else {
	// TBD - what to do for "select" instead of "print" ?
    //append_print_array("|");
    //append_print_array("print");
  }

  while (target_list != NIL) {
    ResTarget  *res = (ResTarget *) lfirst(target_list);

    if (IsA(res->val, FuncCall)) {
      FuncCall * fc = (FuncCall *)res->val;

#ifdef DEBUG
  fprintf(stderr, "CALLING append_array('|') from transformTargetList()\n");
#endif
      append_array("|");

#ifdef DEBUG
      fprintf(stderr,"Found FuncCall in target list: %s\n", fc->funcname);
      fprintf(stderr,"  Calling transformExpr...\n");
#endif

	  transformExpr((Node *) fc, func_list, SELECT_CLAUSE);

      // now unwind the stack, adding 'arg' to each module call
      // eg. COUNT(UNIQ(srcip)) --> UNIQ srcip | COUNT srcip
	  //
#ifdef DEBUG
      fprintf(stderr,"TargetList DEQUEUE LOOP...func_list->num_items = %i\n", func_list->num_items);
#endif

      while ((li = d2_list_dequeue(func_list, FROM_TAIL)) != NULL) {
#ifdef DEBUG
        fprintf(stderr,"TargetList DEQUEUE LOOP...\n");
#endif
        // eg. 'uniq' or 'count'
        append_array(li->str_name);
		arg_list = (list *)li->arg_list;

#ifdef DEBUG
	    fprintf(stderr, "*** TargetList arg_list for li %p = %p\n", li, arg_list);
#endif
		// eg. 'dstip' or '-f cnt dstip'
		if (arg_list) {
		  list_item * ali; // arg list item
		  list_item * pli; // print list item
		  //list_item * ili; // identifier list item

          while ((ali = d2_list_dequeue(arg_list, FROM_HEAD)) != NULL) {
			char * tok;
		    bool COUNT_RENAMED = FALSE; // 'count -f' found?
#ifdef DEBUG
		    fprintf(stderr, "arg for %s = '%s'\n", li->str_name, ali->str_name);
#endif
			// build 'print' list (eg. 'print srcip dstport cnt dstip count')
			//
			if (strcmp(li->str_name, "count") == 0) {

              if (isPrint) {
                append_print_array(ali->str_name); // ??
              }
              append_array(ali->str_name);
              tok = strtok(ali->str_name, " "); 
#ifdef DEBUG
		      fprintf(stderr, "1st tok = '%s'\n", tok);
#endif
			  if (strcmp(tok, "-f") == 0) {
			    tok = strtok(NULL, " "); // eg. 'cnt'
#ifdef DEBUG
		        fprintf(stderr, "2nd tok = '%s'\n", tok);
#endif
			    if (tok) { // eg. 'cnt'
				  pli = make_list_item(tok);
                  if (isPrint) {
                    append_print_array(tok);
                  }
				  COUNT_RENAMED = TRUE;
			    }
			  }
			  else { // not '-f' flag, assume ident
				if (!COUNT_RENAMED) {
                  if (isPrint) {
			        pli = make_list_item("count");
                    append_print_array("count");
				  }
				}
                if (isPrint) {
				  // uncomment next 2 lines to assume that eg. 'print count(x)'
				  // translates to 'print x count', and not just 'print count'
				  //
			      //ili = make_list_item(ali->str_name);
				  //d2_list_enqueue(print_list, ili);

				  d2_list_enqueue(print_list, pli);
			    }
			  }

			}
			else {
              append_array(ali->str_name);
			}

		  }

          if (func_list->num_items > 0) {
#ifdef DEBUG
            fprintf(stderr, "CALLING append_array('|') from transformTargetList()\n");
#endif
            append_array("|");
		  }

		}
	  }
    }
    else if (IsA(res->val, Ident)) {
	  list_item * pli;
      Ident * id = (Ident *) res->val;
#ifdef DEBUG
      fprintf(stderr, "Found Ident in target list: %s\n", id->name);
#endif

      if (isPrint) {
        append_print_array(id->name);
      }

      if (isPrint) {
        pli = make_list_item(id->name); 	
        d2_list_enqueue(print_list, pli);
      }
    }
	else {
      fprintf(stderr, "Unknown target type in target list.\n");
	}

    target_list = lnext(target_list);
  }

#ifdef DEBUG
  if (isPrint) {
    fprintf(stderr, "PRINT LIST = ");
    for (li = print_list->head; li; li = li->next) {
	  fprintf(stderr, "%s ", li->str_name);
    }
    fprintf(stderr, "\n");
  }
#endif

  //d2_list_list_dequeue(func_list_list, FROM_TAIL);

  return rc;
}

int transformFromClause(List * from_list, list * func_list)
{
  int rc;
  List * fl;
  list_item * li;
  list * arg_list;

#ifdef DEBUG
  fprintf(stderr, "Found FROM clause: \n");
#endif

  foreach(fl, from_list) {
    Node * n = lfirst(fl);
#ifdef DEBUG
	fprintf(stderr, "Found node type %d in FROM\n", nodeTag(n));
#endif
    if (IsA(n, RangeVar)) { // TBD - I don't think this will really happen!
      RangeVar * r = (RangeVar *)n;
      char * rel_name = r->relname; // "eg. tcpdumpfile"
#ifdef DEBUG
      fprintf(stderr, "Found RangeVar in FROM: %s\n", rel_name);
#endif
      append_from_array(rel_name);
    }
	else if (IsA(n, SubLink)) {
      SelectStmt * stmt = (SelectStmt *) (((SubLink *)n)->subselect);
#ifdef DEBUG
      fprintf(stderr, "***** Found SubLink in FROM: \n");
      fprintf(stderr,"  Calling transformSelectStmt w/ sub-select...\n");
#endif
      rc = transformSelectStmt(stmt);
	}
	else if (IsA(n, FuncCall)) {
	  FuncCall * fc = (FuncCall *)n; // eg. pcapfile()
#ifdef DEBUG
      fprintf(stderr, "***** Found FuncCall in FROM: %s\n", fc->funcname);
      fprintf(stderr,"  Calling transformExpr...\n");
#endif

	  transformExpr((Node *)fc, func_list, FROM_CLAUSE);

      // now unwind the stack, adding 'arg' to each module call
      // eg. UNIQ srcip COUNT srcip
	  //
      while ((li = d2_list_dequeue(func_list, FROM_TAIL)) != NULL) {
#ifdef DEBUG
        fprintf(stderr,"FromClause DEQUEUE LOOP: li = %s\n", li->str_name);
#endif
        append_from_array(li->str_name);

		arg_list = (list *)li->arg_list;
#ifdef DEBUG
	    fprintf(stderr, "*** FromClause arg_list for li %p = %p\n", li, arg_list);
#endif
		if (arg_list) {
		  list_item * ali;
          while ((ali = d2_list_dequeue(arg_list, FROM_HEAD)) != NULL) {
#ifdef DEBUG
		    fprintf(stderr, "arg for %s = '%s'\n", li->str_name, ali->str_name);
#endif
            append_from_array(ali->str_name);
		  }
		}
	  }
/**
#ifdef DEBUG
      fprintf(stderr, "CALLING append_array('|') from transformFromClause()\n");
#endif
      //append_array("|");
      append_from_array("|");
**/
	}
	else {
      fprintf(stderr, "Found UNKNOWN node type in FROM:\n");
	  return -1;
	}
  }

  return 0;
}

int transformWhereClause(Node *where_clause, list * func_list)
{
  int rc = 0;
  Node * qual;

#ifdef DEBUG
  fprintf(stderr, "Found WHERE clause: \n");
#endif

  append_array("filter");

  qual = transformExpr(where_clause, func_list, WHERE_CLAUSE);

#ifdef DEBUG
  fprintf(stderr, "CALLING append_array('|') from transformWhereClause()\n");
#endif
  append_array("|");

  return rc;
}

int transformGroupClause(List *group_list)
{
  List * gl;

#ifdef DEBUG
  fprintf(stderr, "Found GROUP BY clause: \n");
#endif

  append_array("split");

  foreach(gl, group_list) {
    Node * n = lfirst(gl);
#ifdef DEBUG
    fprintf(stderr, "Found GROUP BY: type = %d\n",nodeTag(n));
#endif
    switch (nodeTag(n)) {
      case T_Ident: {
        Ident * id = (Ident *) n;
#ifdef DEBUG
        fprintf(stderr, "Found Expr in GROUP BY: %s\n", id->name);
#endif
        append_array(id->name);
		//sprintf(cmd, "%s ", id->name);
		//cmd += 1 + strlen(id->name);
        break;
      }
      default: {
        break;
      }
    }
  }
#ifdef DEBUG
  fprintf(stderr, "CALLING append_array('|') from transformGroupClause()\n");
#endif
  append_array("|");
  return 0;
}

int transformSelectStmt(SelectStmt * stmt)
{
  int rc = 0;
  list * func_list;
  char * lname = malloc(sizeof("func_list") + 2);

#ifdef DEBUG
  fprintf(stderr, "Found Select Statement: \n");
#endif

  sprintf(lname, "func_list%i", ++func_list_count);
  func_list = make_list(lname);

  //d2_list_list_enqueue(func_list_list, func_list);

  if (stmt->targetList == NULL) {
    fprintf(stderr, "NULL targetList!\n");
    return -1;
  }
#ifdef DEBUG
  fprintf(stderr, "Calling transformTargetList...\n");
#endif
  rc = transformTargetList(stmt->targetList, func_list, stmt->isPrint);
  if (rc != 0) {
    fprintf(stderr, "Error transforming target list.\n");
    return rc;
  }

  if (stmt->fromClause == NULL) {
    fprintf(stderr, "NULL fromClause!\n");
    return -1;
  }
#ifdef DEBUG
  fprintf(stderr, "Calling transformFromClause...\n");
#endif
  rc = transformFromClause(stmt->fromClause, func_list);
  if (rc != 0) {
    fprintf(stderr, "Error transforming FROM clause.\n");
    return rc;
  }

  if (stmt->groupClause == NULL) {
#ifdef DEBUG
    fprintf(stderr, "NULL groupClause!\n");
#endif
  } 
  else {
#ifdef DEBUG
    fprintf(stderr, "Calling transformGroupClause...\n");
#endif
    rc = transformGroupClause(stmt->groupClause);
    if (rc != 0) {
      fprintf(stderr, "Error transforming GROUP BY clause.\n");
      return rc;
    } 
  }

  if (stmt->whereClause == NULL) {
#ifdef DEBUG
    fprintf(stderr, "NULL whereClause!\n");
#endif
  }
  else {
#ifdef DEBUG
    fprintf(stderr, "Calling transformWhereClause...\n");
#endif
    rc = transformWhereClause(stmt->whereClause, func_list);
    if (rc != 0) {
      fprintf(stderr, "Error transforming WHERE clause.\n");
      return rc;
    }
  }

  //d2_list_list_dequeue(func_list_list, FROM_TAIL);

  return rc;
}

int transformStmt(List * ptree)
{
  int rc = 0;
  List * raw_parsetree_list;
  List * parsetree_item = NULL;

  func_list_count = 0;
  curr_idx = 0;
  curr_print_idx = 0;

  func_list_list = make_list_list("func_list_list");

  raw_parsetree_list = ptree;

  foreach(parsetree_item, raw_parsetree_list) {

    Node * ptree = (Node *)lfirst(parsetree_item);
    SelectStmt * stmt = (SelectStmt *) ptree;

	if (nodeTag(stmt) != T_SelectStmt) {
	  fprintf(stderr, "Not a SELECT or PRINT statement, found type %i.\n", nodeTag(ptree));
	  return -1;
	}

    rc = transformSelectStmt(stmt);
  }

 {
  int i;

#ifdef DEBUG
  create_annotation_variable(0);
  create_annotation_variable(0);
  create_annotation_variable(0);
  create_annotation_variable(3);
  create_annotation_variable(3);
  create_annotation_variable(3);
#endif

  for (i = 0; i < curr_from_idx; i++) {
#ifdef DEBUG
	fprintf(stderr, "i = %d, val = %s\n", i, from_array[i]);
#endif
	append_final_array(from_array[i]);
  }
  for (i = 0; i < curr_idx; i++) {
#ifdef DEBUG
	fprintf(stderr, "i = %d, val = %s\n", i, cmd_array[i]);
#endif
	append_final_array(cmd_array[i]);
  }
  for (i = 0; i < curr_print_idx; i++) {
#ifdef DEBUG
	fprintf(stderr, "i = %d, val = %s\n", i, print_array[i]);
#endif
	append_final_array(print_array[i]);
  }
 }
  
  dump_array();
  dump_final_array();

  // free_all();

  return 0;

}

char ** parse_stmt(int argc, char ** argv, int * new_argc) 
{
  int rc = 0;
  int yyresult;
  char * sqlcmd = malloc(256);

  while (argc > 0) {
	fprintf(stderr, "ARGV = %s\n", *argv);
    sprintf(sqlcmd, "%s ", *argv);
	argc--, argv++;
  }

  scanner_init();

  // needed for the postgres parser
  //
  parseString = sqlcmd;

  // the resulting graph is built into the "parsetree" List
  //
  yyresult = yyparse();

  fprintf(stderr, "\nyyresult = %d\n", yyresult);
  fprintf(stderr, "parsetree = %p\n", parsetree);

  rc = transformStmt(parsetree);

  if (rc != 0) {
	fprintf(stderr, "Error transforming parsetree into SMACQ stmt.\n");
  }

  *new_argc = curr_final_idx; 

  return final_array;
}
