SQLSRC=$(top_srcdir)/libsmacq/sql
INCLUDES = @GLOBAL_INCLUDES@ -I$(top_srcdir)/include 
SUFFIXES = .y .ym
noinst_LTLIBRARIES = libfilter.la
BUILT_SOURCES = 	filter-parser.c expr-parser.c
libfilter_la_SOURCES = 	filter-parser.c expr-parser.c

#LIBS=-lfl
#libfilter.la: $(OBJS)
#	$(LIBTOOL) $(CCLD) -export-symbols-regex '^smacq_.*' -o $@ $+ $(LIBS)

%.y: %.ym $(SQLSRC)/grammer.y
	$(M4) -I$(SQLSRC) -P < $< > $@

filter-%.c: filter-%.y
	$(YACC) -v -p yyfilter -d -o $@ $<

expr-%.c: expr-%.y
	$(YACC) -v -p yyexpr -d -o $@ $<

