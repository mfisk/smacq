SQLSRC=$(top_srcdir)/libsmacq/sql
INCLUDES = @GLOBAL_INCLUDES@ -I$(top_srcdir)/include 
SUFFIXES = .y .ym
noinst_LTLIBRARIES = libfilter.la
BUILT_SOURCES = 	filter-parser.c expr-parser.c
libfilter_la_SOURCES = 	filter-parser.y expr-parser.y

#LIBS=-lfl
#libfilter.la: $(OBJS)
#	$(LIBTOOL) $(CCLD) -export-symbols-regex '^smacq_.*' -o $@ $+ $(LIBS)

$(srcdir)/%.y: $(srcdir)/%.ym $(SQLSRC)/grammer.y
	m4 -I$(SQLSRC) -P -DBUILDDATE=\"`date`\" < $< > $(srcdir)/$@

$(srcdir)/filter-%.c: $(srcdir)/filter-%.y
	$(YACC) -v -p yyfilter -d -o $@ $<

$(srcdir)/expr-%.c: $(srcdir)/expr-%.y
	$(YACC) -v -p yyexpr -d -o $@ $<

