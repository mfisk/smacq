/* A Bison parser, made from filter-parser.y, by GNU bison 1.75.  */

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

#ifndef BISON_FILTER_PARSER_H
# define BISON_FILTER_PARSER_H

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     YYSTOP = 258,
     YYLIKE = 259,
     YYOR = 260,
     YYAND = 261,
     YYSTRING = 262,
     YYID = 263
   };
#endif
#define YYSTOP 258
#define YYLIKE 259
#define YYOR 260
#define YYAND 261
#define YYSTRING 262
#define YYID 263




#ifndef YYSTYPE
#line 38 "filter-parser.y"
typedef union {
  char * string;
  struct list list;
  dts_compare_operation op;
} yystype;
/* Line 1281 of /usr/share/bison/yacc.c.  */
#line 62 "filter-parser.h"
# define YYSTYPE yystype
#endif

extern YYSTYPE yyfilterlval;


#endif /* not BISON_FILTER_PARSER_H */

