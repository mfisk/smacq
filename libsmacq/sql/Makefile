LIBTOOL=libtool
BISON=bison
FLEX=flex

OBJS = scanner.lo parser.lo

CFLAGS = -Wall -I$(SRCDIR) -I$(USR)/include/glib-2.0 -I$(USR)/lib/glib-2.0/include $(INCLUDES) -I$(SRCDIR)/../../include -I. $(COPTS) 

LIBS = -lfl

all: libsql.la parser.c scanner.c

libsql.la: $(OBJS)
	$(LIBTOOL) $(CC) -export-symbols-regex '^smacq_.*' -o $@ $+ $(LIBS)

# We refer explicitly to SRCDIR/%.c so that the bison/flex output will be placed in SRCDIR
%.lo: $(SRCDIR)/%.c
	$(LIBTOOL) $(CC) -c $(CFLAGS) $<

scanner.c:: parser.h

%.y: %.ym grammer.y
	m4 -I$(SRCDIR) -P < $< > $@

%.c: %.y
	$(BISON) -d -o $@ $<

%.h: %.c
	@true

%.c: %.l 
	$(FLEX) -i -o$@ $<

reallyclean: clean
	rm -Rf scanner.c parser.c parser.h scan.c gram.c gram.h

clean:
	rm -Rf *.lo *.la *.o *.a .libs

