/* A Bison parser, made by GNU Bison 1.875a.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     WHERE = 258,
     GROUP = 259,
     BY = 260,
     FROM = 261,
     SELECT = 262,
     AS = 263,
     HAVING = 264,
     UNION = 265,
     YYSTRING = 266,
     YYID = 267,
     YYNUMBER = 268,
     YYNEQ = 269,
     YYLEQ = 270,
     YYGEQ = 271,
     YYSTOP = 272,
     YYLIKE = 273,
     YYOR = 274,
     YYAND = 275,
     YYNOT = 276
   };
#endif
#define WHERE 258
#define GROUP 259
#define BY 260
#define FROM 261
#define SELECT 262
#define AS 263
#define HAVING 264
#define UNION 265
#define YYSTRING 266
#define YYID 267
#define YYNUMBER 268
#define YYNEQ 269
#define YYLEQ 270
#define YYGEQ 271
#define YYSTOP 272
#define YYLIKE 273
#define YYOR 274
#define YYAND 275
#define YYNOT 276




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 105 "/var/tmp/smacq/libsmacq/parser.y"
typedef union YYSTYPE {
  struct graph graph;
  struct arglist * arglist;
  struct vphrase vphrase;
  char * string;
  struct group group;
  dts_compare_operation op;
  enum dts_arith_operand_type arithop;
  dts_comparison * comp;
  struct dts_operand * operand;
} YYSTYPE;
/* Line 1240 of yacc.c.  */
#line 91 "y.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



