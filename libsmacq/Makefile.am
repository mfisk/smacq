INCLUDES = @GLOBAL_INCLUDES@ -I$(top_srcdir)/include
#BUILT_SOURCES = 	scanner.c parser.c parser.y parser.h
EXTRA_DIST = parser.h
#BUILT_SOURCES = 	filter-parser.c expr-parser.c expr-parser.y filter-parser.y
SUFFIXES = .y .ym

noinst_LTLIBRARIES=libsmacq.la @ETHEREAL_LIBRARY@
EXTRA_LTLIBRARIES=#libethereal.la
#libethereal_la_SOURCES=`find @ETHEREAL_DIR@ \( -name plugins -prune \) , \( -name \*.lo -print \)` 

libsmacq_la_SOURCES=mem.c bytehash.c fields.c bloom.c args.c pickle.c types.c sched-thread.c sched-mono.c sched-iter.c construct.c optimize.c sched.c msg.c filter.c module-closure.c dynloader.c parsing.c cmalloc.c parser.y scanner.l filter-parser.y expr-parser.y strucio.c 
libsmacq_la_LIBADD=
libsmacq_la_LDFLAGS = -rpath /bogus

scanner.lo : parser.h

$(srcdir)/%.y: $(srcdir)/%.ym $(srcdir)/grammer.y
	m4 -I$(srcdir) -P -DBUILDDATE="`date`" < $< > $@

%.y: $(srcdir)/%.y
	cp $< $@

