/* A Bison parser, made from /usr/home/mfisk/smacq/libsmacq/sql/parser.y, by GNU bison 1.75.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

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

#ifndef BISON__USR_HOME_MFISK_SMACQ_LIBSMACQ_SQL_PARSER_H
# define BISON__USR_HOME_MFISK_SMACQ_LIBSMACQ_SQL_PARSER_H

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
     YYSTRING = 265,
     YYID = 266,
     YYNEQ = 267,
     YYLEQ = 268,
     YYGEQ = 269,
     YYSTOP = 270,
     YYLIKE = 271,
     YYOR = 272,
     YYAND = 273
   };
#endif
#define WHERE 258
#define GROUP 259
#define BY 260
#define FROM 261
#define SELECT 262
#define AS 263
#define HAVING 264
#define YYSTRING 265
#define YYID 266
#define YYNEQ 267
#define YYLEQ 268
#define YYGEQ 269
#define YYSTOP 270
#define YYLIKE 271
#define YYOR 272
#define YYAND 273




#ifndef YYSTYPE
#line 63 "/usr/home/mfisk/smacq/libsmacq/sql/parser.y"
typedef union {
  struct graph graph;
  struct arglist * arglist;
  struct vphrase vphrase;
  char * string;
  struct group group;
  dts_compare_operation op;
  dts_comparison * comp;
} yystype;
/* Line 1281 of /usr/local/share/bison/yacc.c.  */
#line 86 "/usr/home/mfisk/smacq/libsmacq/sql/parser.h"
# define YYSTYPE yystype
#endif

extern YYSTYPE yysmacql_lval;


#endif /* not BISON__USR_HOME_MFISK_SMACQ_LIBSMACQ_SQL_PARSER_H */

