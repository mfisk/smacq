INCLUDES = @GLOBAL_INCLUDES@ -I$(top_srcdir)/include
EXTRA_DIST = parser.h
SUFFIXES = .ypp .ym

noinst_LTLIBRARIES=libsmacq.la @ETHEREAL_LIBRARY@
EXTRA_LTLIBRARIES=#libethereal.la
#libethereal_la_SOURCES=`find @ETHEREAL_DIR@ \( -name plugins -prune \) , \( -name \*.lo -print \)` 

libsmacq_la_SOURCES=mem.c args.c pickle.cpp optimize.cpp filter.cpp ThreadedSmacqModule.cpp dynloader.c parsing.cpp cmalloc.c parser.ym scanner.lpp filter-parser.ym expr-parser.ym strucio.cpp DTS.cpp DtsObject.cpp smacq_result.cpp 

noinst_HEADERS=grammer.ypp
BUILT_SOURCES=parser.cpp filter-parser.cpp expr-parser.cpp

libsmacq_la_LIBADD=
libsmacq_la_LDFLAGS = -rpath /bogus

filter-parser.ypp: grammer.ypp

expr-parser.ypp: grammer.ypp

parser.ypp: grammer.ypp

.ym.ypp:  grammer.ypp
	cd $(srcdir) && m4 -I$(srcdir) -P -DBUILDDATE="`date`" < $< > $@

