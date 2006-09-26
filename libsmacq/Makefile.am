LIBSMACQ_CPP_SOURCES = smacqq.cpp pickle.cpp optimize.cpp filter.cpp ThreadedSmacqModule.cpp parsing.cpp DTS.cpp DtsObject.cpp smacq_result.cpp am.cpp SmacqScheduler.cpp SmacqGraph.cpp
EXTRA_DIST = parser.hpp $(LIBSMACQ_CPP_SOURCES)
SUFFIXES = .ypp .ym

libsmacq_la_CPPFLAGS = $(AM_CPPFLAGS) -DSMACQ_BUILD_DATE=\"`date '+%Y-%m-%d'`\" 

noinst_LTLIBRARIES=libsmacq.la libscanner.la

libsmacq.cpp: $(LIBSMACQ_CPP_SOURCES)
	for f in $+; do echo "# 1 \"$$f\""; cat $$f; done > $@

# Flex version  2.5.33 (but not 2.5.4) has a sign problem
libscanner_la_CPPFLAGS = $(AM_CPPFLAGS) -Wno-sign-compare
libscanner_la_SOURCES=scanner.lpp

libsmacq_la_SOURCES=libsmacq.cpp mem.c args.c dynloader.c cmalloc.c parser.ym filter-parser.ym expr-parser.ym #datalog-parser.ym
libsmacq_la_LIBADD=libscanner.la @GLIB_LIBS@  $(top_builddir)/modules/libsmacq_modules.la $(top_builddir)/types/libdts_modules.la $(top_builddir)/libgnu/libgnu.la 
libsmacq_la_LDFLAGS = $(AM_LDFLAGS) -rpath /usr/lib -all-static 

noinst_HEADERS=grammer.ypp
BUILT_SOURCES=libsmacq.cpp parser.cpp filter-parser.cpp expr-parser.cpp #datalog-parser.cpp

filter-parser.ypp: grammer.ypp
datalog-parser.ypp: grammer.ypp
expr-parser.ypp: grammer.ypp
parser.ypp: grammer.ypp

.ym.ypp:  grammer.ypp
	m4 -s -I$(srcdir) -P -DBUILDDATE="`date`" $< > $@

.lpp.cpp:
	flex -i -o$@ $<

.ypp.cpp:
	bison -v -d -o $@ $< 

