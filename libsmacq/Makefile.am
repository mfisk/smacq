LIBSMACQ_CPP_SOURCES = smacqq.cpp pickle.cpp optimize.cpp filter.cpp ThreadedSmacqModule.cpp parsing.cpp DTS.cpp DtsObject.cpp smacq_result.cpp am.cpp SmacqScheduler.cpp SmacqGraph.cpp
EXTRA_DIST = parser.hpp $(LIBSMACQ_CPP_SOURCES)
SUFFIXES = .ypp .ym

libsmacq_la_CPPFLAGS = $(AM_CPPFLAGS) -DSMACQ_BUILD_DATE=\"`date '+%Y-%m-%d'`\"  -fPIC

noinst_LTLIBRARIES=libscanner.la libsmacq.la 

libsmacq.cpp: $(LIBSMACQ_CPP_SOURCES)
	for f in $+; do echo "# 1 \"$$f\""; cat $$f; done > $@

# Flex version  2.5.33 (but not 2.5.4) has a sign problem
libscanner_la_CPPFLAGS = $(AM_CPPFLAGS) -Wno-sign-compare
libscanner_la_SOURCES=scanner.lpp

libsmacq_la_SOURCES=libsmacq.cpp mem.c args.c dynloader.c cmalloc.c parser.ym filter-parser.ym expr-parser.ym 
libsmacq_la_LIBADD=libscanner.la @GLIB_LIBS@ @LIBDB_CXX_LIBS@ -lltdl
libsmacq_la_LDFLAGS = $(AM_LDFLAGS) -rpath /usr/lib -all-static -shared

noinst_HEADERS=grammer.ypp
BUILT_SOURCES=libsmacq.cpp parser.cpp filter-parser.cpp expr-parser.cpp preload.h

# preload.h references things so that they will be linked into the library.
# Our module system doesn't statically reference them, but will find them in memory
# if they are loaded.  This is nicer than having to ship a bunch of external module files
# to find at runtime.  So if you include smacq in something (like smacqq or libpysmacq), you
# need only include preload.h to get modules statically linked in.
preload.h: Makefile $(top_srcdir)/misc/preloadsyms ../modules/libsmacq_modules.la ../types/libdts_modules.la
	(nm $(top_builddir)/modules/.libs/libsmacq_modules.a* $(top_builddir)/types/.libs/libdts_modules.a* | egrep -x '[0-9a-fA-F]* [DB] _?(dts_type|smacq)_[^_]*_(table|fields)' | awk '{print $$3};'  | sed 's/^_//' | $(top_srcdir)/misc/preloadsyms > $@) || rm $@

filter-parser.ypp: grammer.ypp
expr-parser.ypp: grammer.ypp
parser.ypp: grammer.ypp

.ym.ypp:  grammer.ypp
	m4 -s -I$(srcdir) -P -DBUILDDATE="`date`" $< > $@

.lpp.cpp:
	flex -i -o$@ $<

.ypp.cpp:
	bison -v -d -o $@ $< 

