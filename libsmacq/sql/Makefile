LIBTOOL=libtool
BISON=bison
FLEX=flex

OBJS = scanner.lo grammer.lo

CFLAGS = -ggdb -Wall -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include $(INCLUDES) -I../../include

LIBS = -lfl

all: libsql.la

libsql.la: $(OBJS)
	$(LIBTOOL) $(CC) -o $@ $+ $(LIBS)

%.lo: %.c
	$(LIBTOOL) $(CC) -c $(CFLAGS) $<

scanner.c:: grammer.h

%.c: %.y
	$(BISON) -d -o $@ $<

%.h: %.c
	@true

%.c: %.l 
	$(FLEX) -o$@ $<

clean: distclean
	rm -Rf scanner.c grammer.c grammer.h scan.c gram.c gram.h

distclean:
	rm -Rf *.lo *.la *.o *.a .libs

