EXTRA_DIST = parser.h
SUFFIXES = .ypp .ym
AM_CXXFLAGS = @CFLAGS@ -DSMACQ_BUILD_DATE=\"`date '+%Y-%m-%d'`\"

noinst_LTLIBRARIES=libsmacq.la @ETHEREAL_LIBRARY@
EXTRA_LTLIBRARIES=libethereal.la
libethereal_la_SOURCES=
libethereal_la_DEPENDENCIES=$(shell find @ETHEREAL_DIR@ \( \-name plugins \-prune \) , \( \-name \*.lo \-print \) ) 

libsmacq_la_SOURCES=smacqq.cpp mem.c args.c pickle.cpp optimize.cpp filter.cpp ThreadedSmacqModule.cpp dynloader.c parsing.cpp cmalloc.c parser.ym scanner.lpp filter-parser.ym expr-parser.ym DTS.cpp DtsObject.cpp smacq_result.cpp am.cpp #datalog-parser.ym

AM_CPPFLAGS=@GLIB_CFLAGS@
noinst_HEADERS=grammer.ypp
BUILT_SOURCES=parser.cpp filter-parser.cpp expr-parser.cpp #datalog-parser.cpp

libsmacq_la_LIBADD=@GLIB_LIBS@ 
libsmacq_la_LDFLAGS = -rpath /bogus

filter-parser.ypp: grammer.ypp

datalog-parser.ypp: grammer.ypp

expr-parser.ypp: grammer.ypp

parser.ypp: grammer.ypp

.ym.ypp:  grammer.ypp
	m4 -s -I$(srcdir) -P -DBUILDDATE="`date`" $< | sed 's|^\(#line.*\)$$|/*\1*/|' > $@

