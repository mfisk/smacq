BISON=bison
M4=m4

include config.mk

OBJS = filter-parser.lo # filter-scanner.lo
SQLSRC=$(TOPSRCDIR)/libsmacq/sql
CFLAGS += -I$(SQLSRC) -I. 
LIBS = -lfl

all: libfilter.la $(SRCDIR)/filter-parser.c $(SRCDIR)/filter-parser.h

libfilter.la: $(OBJS)
	$(LIBTOOL) $(CC) -export-symbols-regex '^smacq_.*' -o $@ $+ $(LIBS)

%.lo: $(SRCDIR)/%.c
	$(LIBTOOL) $(CC) -c $(CFLAGS) $<

$(SRCDIR)/filter-scanner.c: $(SRCDIR)/filter-parser.h

%.y: %.ym $(SQLSRC)/grammer.y
	$(M4) -I$(SQLSRC) -P < $< > $@

%.c: %.y
	$(BISON) -v -p yyfilter -d -o $@ $<

%.h: %.c
	@true

%.c: %.l 
	$(FLEX) -i -o$@ $<

reallyclean: clean
	rm -Rf filter-scanner.c filter-parser.c filter-parser.h 

clean:
	rm -Rf *.lo *.la *.o *.a .libs

