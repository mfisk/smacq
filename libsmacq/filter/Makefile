LIBTOOL=libtool
BISON=bison
FLEX=flex

OBJS = filter-scanner.lo filter-parser.lo

CFLAGS = -ggdb -Wall -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include $(INCLUDES) -I../../include

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

clean: distclean
	rm -Rf filter-scanner.c filter-parser.c filter-parser.h 

distclean:
	rm -Rf *.lo *.la *.o *.a .libs

