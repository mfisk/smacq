LIBTOOL=libtool
BISON=bison
FLEX=flex

OBJS = filter-parser.lo # filter-scanner.lo

CFLAGS = -Wall -I$(SRCDIR) -I$(USR)/include/glib-2.0 -I$(USR)/lib/glib-2.0/include $(INCLUDES) -I$(SRCDIR)/../../include -I$(SRCDIR)/../sql/ -I../sql/ -I. $(COPTS)

LIBS = -lfl

all: libfilter.la $(SRCDIR)/filter-parser.c $(SRCDIR)/filter-parser.h

libfilter.la: $(OBJS)
	$(LIBTOOL) $(CC) -export-symbols-regex '^smacq_.*' -o $@ $+ $(LIBS)

%.lo: $(SRCDIR)/%.c
	$(LIBTOOL) $(CC) -c $(CFLAGS) $<

$(SRCDIR)/filter-scanner.c: $(SRCDIR)/filter-parser.h

%.y: %.ym $(SRCDIR)/../sql/grammer.y
	m4 -I$(SRCDIR)/../sql -P < $< > $@

%.c: %.y
	$(BISON) -p yyfilter -d -o $@ $<

%.h: %.c
	@true

%.c: %.l 
	$(FLEX) -i -o$@ $<

reallyclean: clean
	rm -Rf filter-scanner.c filter-parser.c filter-parser.h 

clean:
	rm -Rf *.lo *.la *.o *.a .libs

