%{

#ifdef SM_DEBUG
#define YYDEBUG 1
#endif
/*-------------------------------------------------------------------------
 *
 * gram.y
 *	  POSTGRES SQL YACC rules/actions
 *
 * Portions Copyright (c) 1996-2001, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  $Header: /cvsroot/smacq/smacq/libsmacq/sql/gram.y,v 1.4 2002/12/12 20:16:55 wbarber Exp $
 *
 * HISTORY
 *	  AUTHOR			DATE			MAJOR EVENT
 *	  Andrew Yu			Sept, 1994		POSTQUEL to SQL conversion
 *	  Andrew Yu			Oct, 1994		lispy code conversion
 *
 * NOTES
 *	  CAPITALS are used to represent terminal symbols.
 *	  non-capitals are used to represent non-terminals.
 *	  SQL92-specific syntax is separated from plain SQL/Postgres syntax
 *	  to help isolate the non-extensible portions of the parser.
 *
 *	  In general, nothing in this file should initiate database accesses
 *	  nor depend on changeable state (such as SET variables).  If you do
 *	  database accesses, your code will fail when we have aborted the
 *	  current transaction and are just parsing commands to find the next
 *	  ROLLBACK or COMMIT.  If you make use of SET variables, then you
 *	  will do the wrong thing in multi-query strings like this:
 *			SET SQL_inheritance TO off; SELECT * FROM foo;
 *	  because the entire string is parsed by gram.y before the SET gets
 *	  executed.  Anything that depends on the database or changeable state
 *	  should be handled inside parse_analyze() so that it happens at the
 *	  right time not the wrong time.  The handling of SQL_inheritance is
 *	  a good example.
 *
 * WARNINGS
 *	  If you use a list, make sure the datum is a node so that the printing
 *	  routines work.
 *
 *	  Sometimes we assign constants to makeStrings. Make sure we don't free
 *	  those.
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include <ctype.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "access/htup.h"
//#include "catalog/pg_type.h"
#include "nodes/params.h"
#include "parsenodes.h" // this now has modified SelectStmt struct (wbarber)
#include "parser/gramparse.h"
//#include "storage/lmgr.h"
//#include "utils/acl.h"
//#include "utils/numeric.h"

#include "pg_list.h"

#ifdef MULTIBYTE
#include "mb/pg_wchar.h"
#else
#define GetStandardEncoding()	0		/* PG_SQL_ASCII */
#define GetStandardEncodingName()	"SQL_ASCII"
#endif

extern List *parsetree;			/* final parse result is delivered here */

static bool QueryIsRule = FALSE;
static Oid	*param_type_info;
static int	pfunc_num_args;


/*
 * If you need access to certain yacc-generated variables and find that
 * they're static by default, uncomment the next line.  (this is not a
 * problem, yet.)
 */
/*#define __YYSCLASS*/

static Node *makeA_Expr(int oper, char *opname, Node *lexpr, Node *rexpr);
/* static Node *makeTypeCast(Node *arg, TypeName *typename); */
static Node *makeStringConst(char *str, TypeName *typename);
/* static Node *makeFloatConst(char *str); */
static Node *makeRowExpr(char *opr, List *largs, List *rargs);
static SelectStmt *findLeftmostSelect(SelectStmt *node);
static Node *makeSetOp(SetOperation op, bool all, Node *larg, Node *rarg);
/* static Node *doNegate(Node *n); */
/* static void doNegateFloat(Value *v); */

#define MASK(b) (1 << (b))

%}


%union
{
	int					ival;
	char				chr;
	char				*str;
	bool				boolean;
	List				*list;
	Node				*node;
	Value				*value;

	Attr				*attr;
	Ident				*ident;

	DefElem				*defelt;
	SortGroupBy			*sortgroupby;
	RangeVar			*range;
	ResTarget			*target;
	ParamNo				*paramno;
}

%type <node>	stmt,
		SelectStmt, 

%type <node>	select_no_parens, select_with_parens, select_clause,
				simple_select

%type <str>		relation_name, 
		attr_name,
		name, func_name

%type <str>
		all_Op, MathOp
		OptUseOp

%type <list>	stmtblock, stmtmulti,
		relation_name_list,
		opt_distinct,
		sort_clause, sortby_list, name_list,
		from_clause, from_list
		expr_list, attrs, target_list, 
		opt_indirection, group_clause,
		trim_list

%type <boolean>	opt_all

%type <node>	where_clause,
				a_expr, b_expr, c_expr, AexprConst,
				in_expr
%type <list>	row_descriptor, row_list, in_expr_nodes
%type <node>	row_expr
%type <boolean>	opt_empty_parentheses
%type <ival>	sub_type
%type <attr>	attr
%type <sortgroupby>		sortby
%type <node>	table_ref
%type <range>	relation_expr
%type <target>	target_el
%type <paramno> ParamNo

%type <ival>	Iconst
%type <str>		Sconst
%type <str>		ColId, ColLabel, type_name
%type <str>		func_name_keyword
%type <str>		reserved_keyword

%type <str>		select_word
%type <node>		data_func

%type <node>	option
%type <list>	option_list

/*
 * If you make any token changes, remember to:
 *		- use "yacc -d" and update parse.h
 *		- update the keyword table in parser/keywords.c
 */

/* Reserved word tokens
 * SQL92 syntax has many type-specific constructs.
 * So, go ahead and make these types reserved words,
 *  and call-out the syntax explicitly.
 * This gets annoying when trying to also retain Postgres' nice
 *  type-extensible features, but we don't really have a choice.
 * - thomas 1997-10-11
 * NOTE: don't forget to add new keywords to the appropriate one of
 * the reserved-or-not-so-reserved keyword lists, below.
 */

/* Keywords (in SQL92 reserved words) */
%token	ALL, AND, ANY, ASC,
		BY,
		DESC, DISTINCT,
		FROM,
		GROUP,
		NOT,
		OR, ORDER,
		PRINT,
		SELECT,
		USING,
		WHERE,

		DELTA, UNIQ, TOP, LAST, SPLIT, COUNTER, 
		DERIVATIVE, PCAPFILE, PCAPLIVE

/* Special keywords, not in the query language - see the "lex" file */
%token <str>	IDENT, FCONST, SCONST, BITCONST, Op
%token <ival>	ICONST, PARAM

/* these are not real. they are here so that they get generated as #define's*/
%token			OP

/* precedence: lowest to highest */
%left		OR
%left		AND
%right		NOT
%right		'='
%nonassoc	'<' '>'
%left		POSTFIXOP		/* dummy for postfix Op rules */
%left		Op				/* multi-character ops and user-defined operators */
%left		'+' '-'
%left		'*' '/' '%'
%left		'^'
/* Unary Operators */
%right		UMINUS
%left		'.'
%left		'[' ']'
%left		'(' ')'
%left		TYPECAST
%%

/*
 *	Handle comment-only lines, and ;; SELECT * FROM pg_class ;;;
 *	psql already handles such cases, but other interfaces don't.
 *	bjm 1999/10/05
 */
stmtblock:  stmtmulti
				{ 
				  parsetree = $1;
				  //fprintf(stderr, "stmtblock: parsetree = %p\n", parsetree); // wbarber
				}
		;

/* the thrashing around here is to discard "empty" statements... */
stmtmulti:  stmtmulti ';' stmt
				{ if ($3 != (Node *)NULL)
					$$ = lappend($1, $3);
				  else
					$$ = $1;
				}
		| stmt
				{ if ($1 != (Node *)NULL)
					$$ = makeList1($1);
				  else
					$$ = NIL;
				}
		;

stmt :	SelectStmt
		| /*EMPTY*/
			{ $$ = (Node *)NULL; }
		;


/*****************************************************************************
 *
 *		QUERY:
 *				SELECT STATEMENTS
 *
 *****************************************************************************/

/* A complete SELECT statement looks like this.
 *
 * The rule returns either a single SelectStmt node or a tree of them,
 * representing a set-operation tree.
 *
 * There is an ambiguity when a sub-SELECT is within an a_expr and there
 * are excess parentheses: do the parentheses belong to the sub-SELECT or
 * to the surrounding a_expr?  We don't really care, but yacc wants to know.
 * To resolve the ambiguity, we are careful to define the grammar so that
 * the decision is staved off as long as possible: as long as we can keep
 * absorbing parentheses into the sub-SELECT, we will do so, and only when
 * it's no longer possible to do that will we decide that parens belong to
 * the expression.  For example, in "SELECT (((SELECT 2)) + 3)" the extra
 * parentheses are treated as part of the sub-select.  The necessity of doing
 * it that way is shown by "SELECT (((SELECT 2)) UNION SELECT 2)".  Had we
 * parsed "((SELECT 2))" as an a_expr, it'd be too late to go back to the
 * SELECT viewpoint when we see the UNION.
 *
 * This approach is implemented by defining a nonterminal select_with_parens,
 * which represents a SELECT with at least one outer layer of parentheses,
 * and being careful to use select_with_parens, never '(' SelectStmt ')',
 * in the expression grammar.  We will then have shift-reduce conflicts
 * which we can resolve in favor of always treating '(' <select> ')' as
 * a select_with_parens.  To resolve the conflicts, the productions that
 * conflict with the select_with_parens productions are manually given
 * precedences lower than the precedence of ')', thereby ensuring that we
 * shift ')' (and then reduce to select_with_parens) rather than trying to
 * reduce the inner <select> nonterminal to something else.  We use UMINUS
 * precedence for this, which is a fairly arbitrary choice.
 *
 * To be able to define select_with_parens itself without ambiguity, we need
 * a nonterminal select_no_parens that represents a SELECT structure with no
 * outermost parentheses.  This is a little bit tedious, but it works.
 *
 * In non-expression contexts, we use SelectStmt which can represent a SELECT
 * with or without outer parentheses.
 */

SelectStmt: select_no_parens			%prec UMINUS
		| select_with_parens			%prec UMINUS
		;

select_with_parens: '(' select_no_parens ')'
			{
				$$ = $2;
			}
		| '(' select_with_parens ')'
			{
				$$ = $2;
			}
		;

select_no_parens: simple_select
			{
				$$ = $1;
			}
		| select_clause sort_clause
			{
				((SelectStmt *)$1)->sortClause = (List *)$2;
				$$ = $1;
			}
		;

select_clause: simple_select
		| select_with_parens
		;

/*
 * This rule parses SELECT statements that can appear within set operations,
 * including UNION, INTERSECT and EXCEPT.  '(' and ')' can be used to specify
 * the ordering of the set operations.  Without '(' and ')' we want the
 * operations to be ordered per the precedence specs at the head of this file.
 *
 * As with select_no_parens, simple_select cannot have outer parentheses,
 * but can have parenthesized subclauses.
 *
 * Note that sort clauses cannot be included at this level --- SQL92 requires
 *		SELECT foo UNION SELECT bar ORDER BY baz
 * to be parsed as
 *		(SELECT foo UNION SELECT bar) ORDER BY baz
 * not
 *		SELECT foo UNION (SELECT bar ORDER BY baz)
 * Likewise FOR UPDATE and LIMIT.  Therefore, those clauses are described
 * as part of the select_no_parens production, not simple_select.
 * This does not limit functionality, because you can reintroduce sort and
 * limit clauses inside parentheses.
 *
 * NOTE: only the leftmost component SelectStmt should have INTO.
 * However, this is not checked by the grammar; parse analysis must check it.
 */
simple_select: 
			 select_word option_list opt_distinct target_list
			 from_clause where_clause
			 group_clause
				{
					SelectStmt *n = makeNode(SelectStmt);
					if (strcmp($1, "print") == 0) {
					  n->isPrint = TRUE;
					}
					else {
					  n->isPrint = FALSE;
					}
					n->functionname = $1;
					n->optionList = $2;
					n->distinctClause = $3;
					n->targetList = $4;
					n->fromClause = $5;
					n->whereClause = $6;
					n->groupClause = $7;
					$$ = (Node *)n;
				}
		;

/*****
simple_select: 
			 select_word opt_distinct target_list
			 from_clause where_clause
			 group_clause
				{
					SelectStmt *n = makeNode(SelectStmt);
					if (strcmp($1, "print") == 0) {
					  n->isPrint = TRUE;
					}
					else {
					  n->isPrint = FALSE;
					}
					n->functionname = $1;
					n->distinctClause = $2;
					n->targetList = $3;
					n->fromClause = $4;
					n->whereClause = $5;
					n->groupClause = $6;
					$$ = (Node *)n;
				}
		;
*****/

select_word: SELECT								{ $$ = "select"; }
		| PRINT									{ $$ = "print"; }
		| UNIQ									{ $$ = "uniq"; }
		| TOP									{ $$ = "top"; }
		| LAST									{ $$ = "last"; }
		;

option_list: option_list option
				{	$$ = lappend($1, $2);  }
		| option
				{	$$ = makeList1($1);  }
		| //EMPTY
				{ $$ = NIL; }
		;

option: AexprConst									{ $$ = (Node *)$1; }
		;

opt_all:  ALL									{ $$ = TRUE; }
		| /*EMPTY*/								{ $$ = FALSE; }
		;

/* We use (NIL) as a placeholder to indicate that all target expressions
 * should be placed in the DISTINCT list during parsetree analysis.
 */
opt_distinct:  DISTINCT							{ $$ = makeList1(NIL); }
		| ALL									{ $$ = NIL; }
		| /*EMPTY*/								{ $$ = NIL; }
		;

sort_clause:  ORDER BY sortby_list				{ $$ = $3; }
		;

sortby_list:  sortby							{ $$ = makeList1($1); }
		| sortby_list ',' sortby				{ $$ = lappend($1, $3); }
		;

sortby: a_expr OptUseOp
				{
					$$ = makeNode(SortGroupBy);
					$$->node = $1;
					$$->useOp = $2;
				}
		;

OptUseOp:  USING all_Op							{ $$ = $2; }
		| ASC									{ $$ = "<"; }
		| DESC									{ $$ = ">"; }
		| /*EMPTY*/								{ $$ = "<"; /*default*/ }
		;


/*
 *	jimmy bell-style recursive queries aren't supported in the
 *	current system.
 *
 *	...however, recursive addattr and rename supported.  make special
 *	cases for these.
 */
relation_name_list:  name_list;

name_list:  name
				{	$$ = makeList1(makeString($1)); }
		| name_list ',' name
				{	$$ = lappend($1, makeString($3)); }
		;

group_clause:  GROUP BY expr_list				
				{ 
				  fprintf(stderr, "group_clause:\n"); 
				  $$ = $3; 
				}
		| /*EMPTY*/								{ $$ = NIL; }
		;

/*****************************************************************************
 *
 *	clauses common to all Optimizable Stmts:
 *		from_clause		- allow list of both JOIN expressions and table names
 *		where_clause	- qualifications for joins or restrictions
 *
 *****************************************************************************/

from_clause:  FROM from_list					{ $$ = $2; }
		| /*EMPTY*/								{ $$ = NIL; }
		;

from_list:  from_list ',' table_ref				{ $$ = lappend($1, $3); }
		| table_ref								{ $$ = makeList1($1); }

		| from_list ',' data_func				{ $$ = lappend($1, $3); }
		| data_func								{ $$ = makeList1($1); }
		;

data_func:
		func_name '(' Sconst ')' // wbarber FNC
				{
					FuncCall *n = makeNode(FuncCall);
					List * al;
					A_Const * c = (A_Const *)makeStringConst($3, NULL);
#ifdef SM_DEBUG
					fprintf(stderr, "table_ref: func_name '(' SCONST ')'\n");
#endif
					al = makeList1(c);
					n->funcname = $1;
					n->args = al;
					n->agg_star = FALSE;
					n->agg_distinct = FALSE;
					$$ = (Node *)n;
				}
		| func_name Sconst // wbarber FNC
				{
					FuncCall *n = makeNode(FuncCall);
					List * al;
					A_Const * c = (A_Const *)makeStringConst($2, NULL);
#ifdef SM_DEBUG
					fprintf(stderr, "table_ref: func_name SCONST\n");
#endif
					al = makeList1(c);
					n->funcname = $1;
					n->args = al;
					n->agg_star = FALSE;
					n->agg_distinct = FALSE;
					$$ = (Node *)n;
				}
		| func_name ColId // wbarber FNC
				{
					FuncCall *n = makeNode(FuncCall);
					List * al;
					A_Const * c = (A_Const *)makeStringConst($2, NULL);
#ifdef SM_DEBUG
					fprintf(stderr, "table_ref: func_name ColId\n");
#endif
					al = makeList1(c);
					n->funcname = $1;
					n->args = al;
					n->agg_star = FALSE;
					n->agg_distinct = FALSE;
					$$ = (Node *)n;
				}
		;

/*
 * table_ref is where an alias clause can be attached.  Note we cannot make
 * alias_clause have an empty production because that causes parse conflicts
 * between table_ref := '(' joined_table ')' alias_clause
 * and joined_table := '(' joined_table ')'.  So, we must have the
 * redundant-looking productions here instead.
 */
table_ref:  
		  relation_expr
				{
					$$ = (Node *) $1;
				}
		| select_with_parens
				{
					/*
					 * The SQL spec does not permit a subselect
					 * (<derived_table>) without an alias clause,
					 * so we don't either.  This avoids the problem
					 * of needing to invent a unique refname for it.
					 * That could be surmounted if there's sufficient
					 * popular demand, but for now let's just implement
					 * the spec and see if anyone complains.
					 * However, it does seem like a good idea to emit
					 * an error message that's better than "parse error".
					 */
					//fprintf(stderr, "sub-SELECT in FROM must have an alias"
					//  "\n\tFor example, FROM (SELECT ...) [AS] foo");
					//$$ = NULL; //wbarber
					SubLink *n = makeNode(SubLink); // wbarber
					n->subselect = $1;
					$$ = (Node *)n;
				}
		;

relation_expr:	relation_name
				{
					/* default inheritance */
					$$ = makeNode(RangeVar);
					$$->relname = $1;
					$$->inhOpt = INH_DEFAULT;
					$$->name = NULL;
				}
		| relation_name '*'
				{
					/* inheritance query */
					$$ = makeNode(RangeVar);
					$$->relname = $1;
					$$->inhOpt = INH_YES;
					$$->name = NULL;
				}
		;

where_clause:  WHERE a_expr						{ $$ = $2; }
		| /*EMPTY*/								{ $$ = NULL;  /* no qualifiers */ }
		;


/*****************************************************************************
 *
 *	expression grammar
 *
 *****************************************************************************/

/* Expressions using row descriptors
 * Define row_descriptor to allow yacc to break the reduce/reduce conflict
 *  with singleton expressions.
 */
row_expr:	'(' row_descriptor ')' all_Op sub_type select_with_parens
				{
					SubLink *n = makeNode(SubLink);
					n->lefthand = $2;
					n->oper = (List *) makeA_Expr(OP, $4, NULL, NULL);
					if (strcmp($4, "<>") == 0)
						n->useor = TRUE;
					else
						n->useor = FALSE;
					n->subLinkType = $5;
					n->subselect = $6;
					$$ = (Node *)n;
				}
		| '(' row_descriptor ')' all_Op select_with_parens
				{
					SubLink *n = makeNode(SubLink);
					n->lefthand = $2;
					n->oper = (List *) makeA_Expr(OP, $4, NULL, NULL);
					if (strcmp($4, "<>") == 0)
						n->useor = TRUE;
					else
						n->useor = FALSE;
					n->subLinkType = MULTIEXPR_SUBLINK;
					n->subselect = $5;
					$$ = (Node *)n;
				}
		| '(' row_descriptor ')' all_Op '(' row_descriptor ')'
				{
					$$ = makeRowExpr($4, $2, $6);
				}
		;

row_descriptor:  row_list ',' a_expr
				{
					$$ = lappend($1, $3);
				}
		;

row_list:  row_list ',' a_expr
				{
					$$ = lappend($1, $3);
				}
		| a_expr
				{
					$$ = makeList1($1);
				}
		;

sub_type:  ANY								{ $$ = ANY_SUBLINK; }
		| ALL								{ $$ = ALL_SUBLINK; }
		;

all_Op:  Op | MathOp;

MathOp:  '+'			{ $$ = "+"; }
		| '-'			{ $$ = "-"; }
		| '*'			{ $$ = "*"; }
		| '/'			{ $$ = "/"; }
		| '%'			{ $$ = "%"; }
		| '^'			{ $$ = "^"; }
		| '<'			{ $$ = "<"; }
		| '>'			{ $$ = ">"; }
		| '='			{ $$ = "="; }
		;

/*
 * General expressions
 * This is the heart of the expression syntax.
 *
 * We have two expression types: a_expr is the unrestricted kind, and
 * b_expr is a subset that must be used in some places to avoid shift/reduce
 * conflicts.  For example, we can't do BETWEEN as "BETWEEN a_expr AND a_expr"
 * because that use of AND conflicts with AND as a boolean operator.  So,
 * b_expr is used in BETWEEN and we remove boolean keywords from b_expr.
 *
 * Note that '(' a_expr ')' is a b_expr, so an unrestricted expression can
 * always be used by surrounding it with parens.
 *
 * c_expr is all the productions that are common to a_expr and b_expr;
 * it's factored out just to eliminate redundant coding.
 */
a_expr:  c_expr
				{	$$ = $1;  }
		/*
		 * These operators must be called out explicitly in order to make use
		 * of yacc/bison's automatic operator-precedence handling.  All other
		 * operator names are handled by the generic productions using "Op",
		 * below; and all those operators will have the same precedence.
		 *
		 * If you add more explicitly-known operators, be sure to add them
		 * also to b_expr and to the MathOp list above.
		 */
		| '+' a_expr					%prec UMINUS
				{	$$ = makeA_Expr(OP, "+", NULL, $2); }
		| '-' a_expr					%prec UMINUS
				{	/* $$ = doNegate($2); */ }
		| '%' a_expr
				{	$$ = makeA_Expr(OP, "%", NULL, $2); }
		| '^' a_expr
				{	$$ = makeA_Expr(OP, "^", NULL, $2); }
		| a_expr '%'
				{	$$ = makeA_Expr(OP, "%", $1, NULL); }
		| a_expr '^'
				{	$$ = makeA_Expr(OP, "^", $1, NULL); }
		| a_expr '+' a_expr
				{	$$ = makeA_Expr(OP, "+", $1, $3); }
		| a_expr '-' a_expr
				{	$$ = makeA_Expr(OP, "-", $1, $3); }
		| a_expr '*' a_expr
				{	$$ = makeA_Expr(OP, "*", $1, $3); }
		| a_expr '/' a_expr
				{	$$ = makeA_Expr(OP, "/", $1, $3); }
		| a_expr '%' a_expr
				{	$$ = makeA_Expr(OP, "%", $1, $3); }
		| a_expr '^' a_expr
				{	$$ = makeA_Expr(OP, "^", $1, $3); }
		| a_expr '<' a_expr
				{	$$ = makeA_Expr(OP, "<", $1, $3); }
		| a_expr '>' a_expr
				{	$$ = makeA_Expr(OP, ">", $1, $3); }
		| a_expr '=' a_expr
				{	$$ = makeA_Expr(OP, "=", $1, $3); }

		| a_expr Op a_expr
				{	$$ = makeA_Expr(OP, $2, $1, $3); }
		| Op a_expr
				{	$$ = makeA_Expr(OP, $1, NULL, $2); }
		| a_expr Op					%prec POSTFIXOP
				{	$$ = makeA_Expr(OP, $2, $1, NULL); }

		| a_expr AND a_expr
				{	$$ = makeA_Expr(AND, NULL, $1, $3); }
		| a_expr OR a_expr
				{	$$ = makeA_Expr(OR, NULL, $1, $3); }
		| NOT a_expr
				{	$$ = makeA_Expr(NOT, NULL, NULL, $2); }

		| a_expr all_Op sub_type select_with_parens		%prec Op
				{
					SubLink *n = makeNode(SubLink);
					n->lefthand = makeList1($1);
					n->oper = (List *) makeA_Expr(OP, $2, NULL, NULL);
					n->useor = FALSE; /* doesn't matter since only one col */
					n->subLinkType = $3;
					n->subselect = $4;
					$$ = (Node *)n;
				}
		| row_expr
				{	$$ = $1;  }
		;

/*
 * Restricted expressions
 *
 * b_expr is a subset of the complete expression syntax defined by a_expr.
 *
 * Presently, AND, NOT, IS, and IN are the a_expr keywords that would
 * cause trouble in the places where b_expr is used.  For simplicity, we
 * just eliminate all the boolean-keyword-operator productions from b_expr.
 */
b_expr:  c_expr
				{	$$ = $1;  }
		| '+' b_expr					%prec UMINUS
				{	$$ = makeA_Expr(OP, "+", NULL, $2); }
		| '-' b_expr					%prec UMINUS
				{	/* $$ = doNegate($2); */ }
		| '%' b_expr
				{	$$ = makeA_Expr(OP, "%", NULL, $2); }
		| '^' b_expr
				{	$$ = makeA_Expr(OP, "^", NULL, $2); }
		| b_expr '%'
				{	$$ = makeA_Expr(OP, "%", $1, NULL); }
		| b_expr '^'
				{	$$ = makeA_Expr(OP, "^", $1, NULL); }
		| b_expr '+' b_expr
				{	$$ = makeA_Expr(OP, "+", $1, $3); }
		| b_expr '-' b_expr
				{	$$ = makeA_Expr(OP, "-", $1, $3); }
		| b_expr '*' b_expr
				{	$$ = makeA_Expr(OP, "*", $1, $3); }
		| b_expr '/' b_expr
				{	$$ = makeA_Expr(OP, "/", $1, $3); }
		| b_expr '%' b_expr
				{	$$ = makeA_Expr(OP, "%", $1, $3); }
		| b_expr '^' b_expr
				{	$$ = makeA_Expr(OP, "^", $1, $3); }
		| b_expr '<' b_expr
				{	$$ = makeA_Expr(OP, "<", $1, $3); }
		| b_expr '>' b_expr
				{	$$ = makeA_Expr(OP, ">", $1, $3); }
		| b_expr '=' b_expr
				{	$$ = makeA_Expr(OP, "=", $1, $3); }

		| b_expr Op b_expr
				{	$$ = makeA_Expr(OP, $2, $1, $3); }
		| Op b_expr
				{	$$ = makeA_Expr(OP, $1, NULL, $2); }
		| b_expr Op					%prec POSTFIXOP
				{	$$ = makeA_Expr(OP, $2, $1, NULL); }
		;

/*
 * Productions that can be used in both a_expr and b_expr.
 *
 * Note: productions that refer recursively to a_expr or b_expr mostly
 * cannot appear here.  However, it's OK to refer to a_exprs that occur
 * inside parentheses, such as function arguments; that cannot introduce
 * ambiguity to the b_expr syntax.
 */
c_expr:  attr
				{	$$ = (Node *) $1;  }
		| ColId opt_indirection
				{
					/* could be a column name or a relation_name */
					Ident *n = makeNode(Ident);
					n->name = $1;
					n->indirection = $2;
					$$ = (Node *)n;
				}
		| AexprConst
				{	$$ = $1;  }
		| '(' a_expr ')'
				{	$$ = $2; }
		| func_name '(' ')'
				{
					FuncCall *n = makeNode(FuncCall);
					n->funcname = $1;
					n->args = NIL;
					n->agg_star = FALSE;
					n->agg_distinct = FALSE;
					$$ = (Node *)n;
				}
		| func_name '(' Sconst ')' // wbarber FNC
				{
					FuncCall *n = makeNode(FuncCall);
					List * al;
					A_Const * c = (A_Const *)makeStringConst($3, NULL);
					al = makeList1(c); // wbarber
					fprintf(stderr, "c_expr: func_name '(' SCONST ')'\n");
					n->funcname = $1;
					n->args = al;
					n->agg_star = FALSE;
					n->agg_distinct = FALSE;
					$$ = (Node *)n;
				}
		| func_name Sconst  // wbarber FNC
				{
					FuncCall *n = makeNode(FuncCall);
					List * al;
					A_Const * c = (A_Const *)makeStringConst($2, NULL);
					al = makeList1(c); // wbarber
					fprintf(stderr, "c_expr: func_name SCONST \n");
					n->funcname = $1;
					n->args = al;
					n->agg_star = FALSE;
					n->agg_distinct = FALSE;
					$$ = (Node *)n;
				}
		| func_name ColId  // wbarber FNC
				{
					FuncCall *n = makeNode(FuncCall);
					List * al;
					A_Const * c = (A_Const *)makeStringConst($2, NULL);
					al = makeList1(c); // wbarber
					fprintf(stderr, "c_expr: func_name ColId \n");
					n->funcname = $1;
					n->args = al;
					n->agg_star = FALSE;
					n->agg_distinct = FALSE;
					$$ = (Node *)n;
				}
		| func_name '(' expr_list ')'
				{
					FuncCall *n = makeNode(FuncCall);
					n->funcname = $1;
					n->args = $3;
					n->agg_star = FALSE;
					n->agg_distinct = FALSE;
					$$ = (Node *)n;
				}
		| func_name '(' ALL expr_list ')'
				{
					FuncCall *n = makeNode(FuncCall);
					n->funcname = $1;
					n->args = $4;
					n->agg_star = FALSE;
					n->agg_distinct = FALSE;
					/* Ideally we'd mark the FuncCall node to indicate
					 * "must be an aggregate", but there's no provision
					 * for that in FuncCall at the moment.
					 */
					$$ = (Node *)n;
				}
		| func_name '(' DISTINCT expr_list ')'
				{
					FuncCall *n = makeNode(FuncCall);
					n->funcname = $1;
					n->args = $4;
					n->agg_star = FALSE;
					n->agg_distinct = TRUE;
					$$ = (Node *)n;
				}
		| func_name '(' '*' ')'
				{
					/*
					 * For now, we transform AGGREGATE(*) into AGGREGATE(1).
					 *
					 * This does the right thing for COUNT(*) (in fact,
					 * any certainly-non-null expression would do for COUNT),
					 * and there are no other aggregates in SQL92 that accept
					 * '*' as parameter.
					 *
					 * The FuncCall node is also marked agg_star = true,
					 * so that later processing can detect what the argument
					 * really was.
					 */
					FuncCall *n = makeNode(FuncCall);
					A_Const *star = makeNode(A_Const);

					star->val.type = T_Integer;
					star->val.val.ival = 1;
					n->funcname = $1;
					n->args = makeList1(star);
					n->agg_star = TRUE;
					n->agg_distinct = FALSE;
					$$ = (Node *)n;
				}
		| select_with_parens			%prec UMINUS
				{
					SubLink *n = makeNode(SubLink);
					n->lefthand = NIL;
					n->oper = NIL;
					n->useor = FALSE;
					n->subLinkType = EXPR_SUBLINK;
					n->subselect = $1;
					$$ = (Node *)n;
				}
		;

/*
 * Supporting nonterminals for expressions.
 */

opt_indirection:	opt_indirection '[' a_expr ']'
				{
					A_Indices *ai = makeNode(A_Indices);
					ai->lidx = NULL;
					ai->uidx = $3;
					$$ = lappend($1, ai);
				}
		| opt_indirection '[' a_expr ':' a_expr ']'
				{
					A_Indices *ai = makeNode(A_Indices);
					ai->lidx = $3;
					ai->uidx = $5;
					$$ = lappend($1, ai);
				}
		| /*EMPTY*/
				{	$$ = NIL; }
		;

expr_list:  a_expr
				{ $$ = makeList1($1); }
		| expr_list ',' a_expr
				{ $$ = lappend($1, $3); }
		;

trim_list:  a_expr FROM expr_list
				{ $$ = lappend($3, $1); }
		| FROM expr_list
				{ $$ = $2; }
		| expr_list
				{ $$ = $1; }
		;

in_expr:  select_with_parens
				{
					SubLink *n = makeNode(SubLink);
					n->subselect = $1;
					$$ = (Node *)n;
				}
		| '(' in_expr_nodes ')'
				{	$$ = (Node *)$2; }
		;

in_expr_nodes:  a_expr
				{	$$ = makeList1($1); }
		| in_expr_nodes ',' a_expr
				{	$$ = lappend($1, $3); }
		;

attr:  relation_name '.' attrs opt_indirection
				{
					$$ = makeNode(Attr);
					$$->relname = $1;
					$$->paramNo = NULL;
					$$->attrs = $3;
					$$->indirection = $4;
				}
		| ParamNo '.' attrs opt_indirection
				{
					$$ = makeNode(Attr);
					$$->relname = NULL;
					$$->paramNo = $1;
					$$->attrs = $3;
					$$->indirection = $4;
				}
		;

attrs:	  attr_name
				{ $$ = makeList1(makeString($1)); }
		| attrs '.' attr_name
				{ $$ = lappend($1, makeString($3)); }
		| attrs '.' '*'
				{ $$ = lappend($1, makeString("*")); }
		;

opt_empty_parentheses: '(' ')' { $$ = TRUE; }
					| /*EMPTY*/ { $$ = TRUE; }
		;

/*****************************************************************************
 *
 *	target lists
 *
 *****************************************************************************/

/* Target lists as found in SELECT ... and INSERT VALUES ( ... ) */

target_list:  target_list ',' target_el
				{	$$ = lappend($1, $3);  }
		| target_el
				{	$$ = makeList1($1);  }
		;

/* AS is not optional because shift/red conflict with unary ops */
target_el:	a_expr
				{
					$$ = makeNode(ResTarget);
					$$->name = NULL;
					$$->indirection = NULL;
					$$->val = (Node *)$1;
				}
		| Sconst 
				{
					$$ = makeNode(ResTarget);
					$$->name = NULL;
					$$->indirection = NULL;
					$$->val = (Node *)$1;
					//Ident *n = makeNode(Ident);
					//n->name = $1;
					//n->indirection = $2;
					//$$ = (Node *)n;
				}
		| relation_name '.' '*'
				{
					Attr *att = makeNode(Attr);
					att->relname = $1;
					att->paramNo = NULL;
					att->attrs = makeList1(makeString("*"));
					att->indirection = NIL;
					$$ = makeNode(ResTarget);
					$$->name = NULL;
					$$->indirection = NULL;
					$$->val = (Node *)att;
				}
		| '*'
				{
					Attr *att = makeNode(Attr);
					att->relname = "*";
					att->paramNo = NULL;
					att->attrs = NULL;
					att->indirection = NIL;
					$$ = makeNode(ResTarget);
					$$->name = NULL;
					$$->indirection = NULL;
					$$->val = (Node *)att;
				}
		;

/*****************************************************************************
 *
 *	Names and constants
 *
 *****************************************************************************/

relation_name:	ColId
				{
					$$ = $1;
				}
		;

name:					ColId			{ $$ = $1; };
attr_name:				ColId			{ $$ = $1; };

/* Constants
 * Include TRUE/FALSE for SQL3 support. - thomas 1997-10-24
 */
AexprConst:  Iconst
				{
					A_Const *n = makeNode(A_Const);
					n->val.type = T_Integer;
					n->val.val.ival = $1;
					$$ = (Node *)n;
				}
		| FCONST
				{
					A_Const *n = makeNode(A_Const);
					n->val.type = T_Float;
					n->val.val.str = $1;
					$$ = (Node *)n;
				}
		| Sconst
				{
					A_Const *n = makeNode(A_Const);
					n->val.type = T_String;
					n->val.val.str = $1;
					$$ = (Node *)n;
				}
		| ParamNo
				{	$$ = (Node *)$1;  }
		;

ParamNo:  PARAM opt_indirection
				{
					$$ = makeNode(ParamNo);
					$$->number = $1;
					$$->indirection = $2;
				}
		;

Iconst:  ICONST							{ $$ = $1; };
Sconst:  SCONST							{ $$ = $1; };

/*
 * Name classification hierarchy.
 *
 * IDENT is the lexeme returned by the lexer for identifiers that match
 * no known keyword.  In most cases, we can accept certain keywords as
 * names, not only IDENTs.  We prefer to accept as many such keywords
 * as possible to minimize the impact of "reserved words" on programmers.
 * So, we divide names into several possible classes.  The classification
 * is chosen in part to make keywords acceptable as names wherever possible.
 */

/* Column identifier --- names that can be column, table, etc names.
 */
ColId:  IDENT							{ $$ = $1; }
		;

/* Type identifier --- names that can be type names.
 */
type_name:  IDENT						{ $$ = $1; }
		;

/* Function identifier --- names that can be function names.
 */
func_name:  IDENT						{ $$ = $1; }
		| func_name_keyword				{ $$ = $1; }
		;

/* Column label --- allowed labels in "AS" clauses.
 * This presently includes *all* Postgres keywords.
 */
ColLabel:  IDENT						{ $$ = $1; }
		| func_name_keyword				{ $$ = $1; }
		| reserved_keyword				{ $$ = $1; }
		;


/*
 * Keyword classification lists.  Generally, every keyword present in
 * the Postgres grammar should appear in exactly one of these lists.
 *
 * Put a new keyword into the first list that it can go into without causing
 * shift or reduce conflicts.  The earlier lists define "less reserved"
 * categories of keywords.
 */

/* "Unreserved" keywords --- available for use as any kind of name.
 */

/* Column identifier --- keywords that can be column, table, etc names.
 *
 * Many of these keywords will in fact be recognized as type or function
 * names too; but they have special productions for the purpose, and so
 * can't be treated as "generic" type or function names.
 *
 * The type names appearing here are not usable as function names
 * because they can be followed by '(' in typename productions, which
 * looks too much like a function call for an LR(1) parser.
 */

/* Function identifier --- keywords that can be function names.
 *
 * Most of these are keywords that are used as operators in expressions;
 * in general such keywords can't be column names because they would be
 * ambiguous with variables, but they are unambiguous as function identifiers.
 *
 * Do not include POSITION, SUBSTRING, etc here since they have explicit
 * productions in a_expr to support the goofy SQL9x argument syntax.
 *  - thomas 2000-11-28
 */

func_name_keyword:
		  COUNTER							{ $$ = "counter"; }
		| DERIVATIVE						{ $$ = "derivative"; }
		| PCAPFILE							{ $$ = "pcapfile"; }
		| PCAPLIVE							{ $$ = "pcaplive"; }
		| SPLIT							{ $$ = "split"; }
		| DELTA							{ $$ = "delta"; }
		//| UNIQ							{ $$ = "uniq"; }
		//| TOP							{ $$ = "top"; }
		//| LAST							{ $$ = "last"; }
		;

/* Reserved keyword --- these keywords are usable only as a ColLabel.
 *
 * Keywords appear here if they could not be distinguished from variable,
 * type, or function names in some contexts.  Don't put things here unless
 * forced to.
 *

 * NOTE! If you add or remove keywords from this list, you MUST
 * also add/remove them from the list in "keywords.c" !
 * -- wbarber
 */
reserved_keyword:
		  ALL							{ $$ = "all"; }
		| AND							{ $$ = "and"; }
		| ANY							{ $$ = "any"; }
		| ASC							{ $$ = "asc"; }
		| DESC							{ $$ = "desc"; }
		| DISTINCT						{ $$ = "distinct"; }
		| FROM							{ $$ = "from"; }
		| GROUP							{ $$ = "group"; }
		| LAST							{ $$ = "last"; }
		| NOT							{ $$ = "not"; }
		| OR							{ $$ = "or"; }
		| ORDER							{ $$ = "order"; }
		| PRINT							{ $$ = "print"; }
		| SELECT						{ $$ = "select"; }
		| TOP							{ $$ = "top"; }
		| UNIQ							{ $$ = "uniq"; }
		| USING							{ $$ = "using"; }
		| WHERE							{ $$ = "where"; }
		;


%%

static Node *
makeA_Expr(int oper, char *opname, Node *lexpr, Node *rexpr)
{
	A_Expr *a = makeNode(A_Expr);
	a->oper = oper;
	a->opname = opname;
	a->lexpr = lexpr;
	a->rexpr = rexpr;
	return (Node *)a;
}

static Node *
makeTypeCast(Node *arg, TypeName *typename)
{
	/*
	 * If arg is an A_Const or ParamNo, just stick the typename into the
	 * field reserved for it --- unless there's something there already!
	 * (We don't want to collapse x::type1::type2 into just x::type2.)
	 * Otherwise, generate a TypeCast node.
	 */
	if (IsA(arg, A_Const) &&
		((A_Const *) arg)->typename == NULL)
	{
		((A_Const *) arg)->typename = typename;
		return arg;
	}
	else if (IsA(arg, ParamNo) &&
			 ((ParamNo *) arg)->typename == NULL)
	{
		((ParamNo *) arg)->typename = typename;
		return arg;
	}
	else
	{
		TypeCast *n = makeNode(TypeCast);
		n->arg = arg;
		n->typename = typename;
		return (Node *) n;
	}
}

static Node *
makeStringConst(char *str, TypeName *typename)
{
	A_Const *n = makeNode(A_Const);
	n->val.type = T_String;
	n->val.val.str = str;
	n->typename = typename;

	return (Node *)n;
}

/* makeRowExpr()
 * Generate separate operator nodes for a single row descriptor expression.
 * Perhaps this should go deeper in the parser someday...
 * - thomas 1997-12-22
 */
static Node *
makeRowExpr(char *opr, List *largs, List *rargs)
{
	Node *expr = NULL;
	Node *larg, *rarg;

	if (length(largs) != length(rargs))
		fprintf(stderr,"Unequal number of entries in row expression");

	if (lnext(largs) != NIL)
		expr = makeRowExpr(opr,lnext(largs),lnext(rargs));

	larg = lfirst(largs);
	rarg = lfirst(rargs);

	if ((strcmp(opr, "=") == 0)
	 || (strcmp(opr, "<") == 0)
	 || (strcmp(opr, "<=") == 0)
	 || (strcmp(opr, ">") == 0)
	 || (strcmp(opr, ">=") == 0))
	{
		if (expr == NULL)
			expr = makeA_Expr(OP, opr, larg, rarg);
		else
			expr = makeA_Expr(AND, NULL, expr, makeA_Expr(OP, opr, larg, rarg));
	}
	else if (strcmp(opr, "<>") == 0)
	{
		if (expr == NULL)
			expr = makeA_Expr(OP, opr, larg, rarg);
		else
			expr = makeA_Expr(OR, NULL, expr, makeA_Expr(OP, opr, larg, rarg));
	}
	else
	{
		fprintf(stderr,"Operator '%s' not implemented for row expressions",opr);
	}

	return expr;
}

/* findLeftmostSelect()
 *		Find the leftmost component SelectStmt in a set-operation parsetree.
 */
static SelectStmt *
findLeftmostSelect(SelectStmt *node)
{
	while (node && node->op != SETOP_NONE)
		node = node->larg;
	//Assert(node && IsA(node, SelectStmt) && node->larg == NULL);
	return node;
}

static Node *
makeSetOp(SetOperation op, bool all, Node *larg, Node *rarg)
{
	SelectStmt *n = makeNode(SelectStmt);

	n->op = op;
	n->all = all;
	n->larg = (SelectStmt *) larg;
	n->rarg = (SelectStmt *) rarg;
	return (Node *) n;
}

void parser_init(Oid *typev, int nargs)
{
	QueryIsRule = FALSE;
	/*
	 * Keep enough information around to fill out the type of param nodes
	 * used in postquel functions
	 */
	param_type_info = typev;
	pfunc_num_args = nargs;
}

Oid param_type(int t)
{
	if ((t > pfunc_num_args) || (t <= 0))
		return InvalidOid;
	return param_type_info[t - 1];
}

/*
 * Test whether an a_expr is a plain NULL constant or not.
 */
bool
exprIsNullConstant(Node *arg)
{
	if (arg && IsA(arg, A_Const))
	{
		A_Const *con = (A_Const *) arg;

		if (con->val.type == T_Null &&
			con->typename == NULL)
			return TRUE;
	}
	return FALSE;
}
