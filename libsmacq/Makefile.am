#LDFLAGS=-lgmodule-2.0 -lglib-2.0 -lm $(LDOPTS)

SUBDIRS = sql filter
INCLUDES = @GLOBAL_INCLUDES@ -I$(top_srcdir)/include

noinst_LTLIBRARIES=libsmacq.la
libsmacq_la_SOURCES=mem.c bytehash.c fields.c bloom.c args.c pickle.c types.c sched-thread.c sched-mono.c sched-iter.c construct.c optimize.c sched.c msg.c filter.c module-closure.c dynloader.c parsing.c cmalloc.c
libsmacq_la_LIBADD=sql/libsql.la filter/libfilter.la 


