INCLUDES = @GLOBAL_INCLUDES@ -I$(top_srcdir)/include
#BUILT_SOURCES = 	scanner.c parser.c parser.y
#BUILT_SOURCES = 	filter-parser.c expr-parser.c expr-parser.y filter-parser.y
SUFFIXES = .ypp .ym

noinst_LTLIBRARIES=libsmacq.la @ETHEREAL_LIBRARY@
EXTRA_LTLIBRARIES=#libethereal.la
#libethereal_la_SOURCES=`find @ETHEREAL_DIR@ \( -name plugins -prune \) , \( -name \*.lo -print \)` 

libsmacq_la_SOURCES=mem.c bytehash.c fields.cpp bloom.c args.c pickle.cpp sched-iter.cpp construct.cpp optimize.c sched.cpp msg.cpp filter.cpp ThreadedSmacqModule.cpp dynloader.c parsing.cpp cmalloc.c parser.ym scanner.lpp filter-parser.ym expr-parser.ym strucio.cpp SmacqModule.cpp SmacqModuleWrapper.cpp DTS.cpp DtsObject.cpp

libsmacq_la_LIBADD=
libsmacq_la_LDFLAGS = -rpath /bogus

.ym.ypp:  grammer.ypp
	cd $(srcdir) && m4 -I$(srcdir) -P -DBUILDDATE="`date`" < $< > $@

