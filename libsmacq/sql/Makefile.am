FLEXFLAGS = -Pyysmacql_
noinst_LTLIBRARIES = libsql.la
INCLUDES = @GLOBAL_INCLUDES@ -I$(top_srcdir)/include
BUILT_SOURCES = 	scanner.c parser.c
libsql_la_SOURCES =	scanner.l parser.y

SUFFIXES = .y .ym

#LIBS = -lfl
#libsql.la: $(OBJS)
#	$(LIBTOOL) $(CCLD) -export-symbols-regex '^smacq_.*' -o $@ $+ $(LIBS)

%.y: %.ym grammer.y
	m4 -I$(srcdir) -P < $< > $@


