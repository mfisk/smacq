EXTRA_DIST = parser.h
SUFFIXES = .ypp .ym

libsmacq_la_CPPFLAGS = $(AM_CPPFLAGS) -DSMACQ_BUILD_DATE=\"`date '+%Y-%m-%d'`\" 

noinst_LTLIBRARIES=libsmacq.la 

libsmacq.cpp: smacqq.cpp pickle.cpp optimize.cpp filter.cpp ThreadedSmacqModule.cpp parsing.cpp DTS.cpp DtsObject.cpp smacq_result.cpp am.cpp SmacqScheduler.cpp SmacqGraph.cpp
	for f in $+; do echo "# 1 \"$$f\""; cat $$f; done > $@

libsmacq_la_SOURCES=scanner.lpp libsmacq.cpp mem.c args.c dynloader.c cmalloc.c parser.ym filter-parser.ym expr-parser.ym #datalog-parser.ym
libsmacq_la_LIBADD=@GLIB_LIBS@  $(top_builddir)/modules/libsmacq_modules.la $(top_builddir)/types/libdts_modules.la $(top_builddir)/libgnu/libgnu.la 
libsmacq_la_LDFLAGS = $(AM_LDFLAGS) -lltdl -rpath /usr/lib -all-static

noinst_HEADERS=grammer.ypp
BUILT_SOURCES=parser.cpp filter-parser.cpp expr-parser.cpp #datalog-parser.cpp

filter-parser.ypp: grammer.ypp
datalog-parser.ypp: grammer.ypp
expr-parser.ypp: grammer.ypp
parser.ypp: grammer.ypp

.ym.ypp:  grammer.ypp
	m4 -s -I$(srcdir) -P -DBUILDDATE="`date`" $< > $@

.lpp.cpp:
	flex -i -o$@ $<

.ypp.cpp:
	bison -d -o $@ $< 

