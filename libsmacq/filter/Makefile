BISON=bison
M4=m4

include config.mk

OBJS = filter-parser.lo expr-parser.lo # filter-scanner.lo
SQLSRC=$(TOPSRCDIR)/libsmacq/sql
CFLAGS += -I$(SQLSRC) -I. 
LIBS = -lfl

all: libfilter.la $(SRCDIR)/filter-parser.c $(SRCDIR)/expr-parser.c $(SRCDIR)/expr-parser.h $(SRCDIR)/filter-parser.h

libfilter.la: $(OBJS)
	$(LIBTOOL) $(CC) -export-symbols-regex '^smacq_.*' -o $@ $+ $(LIBS)

%.lo: $(SRCDIR)/%.c
	$(LIBTOOL) $(CC) -c $(CFLAGS) $<

$(SRCDIR)/%-scanner.c: $(SRCDIR)/%-parser.h

%.y: %.ym $(SQLSRC)/grammer.y
	$(M4) -I$(SQLSRC) -P < $< > $@

filter-%.c: filter-%.y
	$(BISON) -v -p yyfilter -d -o $@ $<


expr-%.c: expr-%.y
	$(BISON) -v -p yyexpr -d -o $@ $<

%.h: %.c
	@true

%.c: %.l 
	$(FLEX) -i -o$@ $<

reallyclean: clean
	rm -Rf filter-scanner.c filter-parser.c filter-parser.h 

clean:
	rm -Rf *.lo *.la *.o *.a .libs

