LIBTOOL=libtool
BISON=bison
FLEX=flex

OBJS = filter-scanner.lo filter-parser.lo

CFLAGS = -Wall -I$(SRCDIR) -I$(USR)/include/glib-2.0 -I$(USR)/lib/glib-2.0/include $(INCLUDES) -I$(SRCDIR)/../../include $(COPTS)

LIBS = -lfl

all: libfilter.la

libfilter.la: $(OBJS)
	$(LIBTOOL) $(CC) -export-symbols-regex '^smacq_.*' -o $@ $+ $(LIBS)

%.lo: %.c
	$(LIBTOOL) $(CC) -c $(CFLAGS) $<

filter-scanner.c: filter-parser.h

%.c: %.y
	$(BISON) -p yyfilter -d -o $@ $<

%.h: %.c
	@true

%.c: %.l 
	$(FLEX) -i -Pyyfilter -o$@ $<

reallyclean: clean
	rm -Rf filter-scanner.c filter-parser.c filter-parser.h 

clean:
	rm -Rf *.lo *.la *.o *.a .libs

