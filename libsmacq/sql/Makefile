LIBTOOL=libtool
BISON=bison
FLEX=flex

OBJS = scanner.lo grammer.lo

CFLAGS = -Wall -I$(USR)/include/glib-2.0 -I$(USR)/lib/glib-2.0/include $(INCLUDES) -I../../include $(COPTS)

LIBS = -lfl

all: libsql.la grammer.c scanner.c

libsql.la: $(OBJS)
	$(LIBTOOL) $(CC) -export-symbols-regex '^smacq_.*' -o $@ $+ $(LIBS)

%.lo: %.c
	$(LIBTOOL) $(CC) -c $(CFLAGS) $<

scanner.c:: grammer.h

%.c: %.y
	$(BISON) -d -o $@ $<

%.h: %.c
	@true

%.c: %.l 
	$(FLEX) -i -o$@ $<

reallyclean: clean
	rm -Rf scanner.c grammer.c grammer.h scan.c gram.c gram.h

clean:
	rm -Rf *.lo *.la *.o *.a .libs

