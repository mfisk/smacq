BISON=bison
FLEX=flex
M4=m4
include config.mk

OBJS = scanner.lo parser.lo
LIBS = -lfl

all: libsql.la $(SRCDIR)/parser.c $(SRCDIR)/scanner.c

libsql.la: $(OBJS)
	$(LIBTOOL) $(CC) -export-symbols-regex '^smacq_.*' -o $@ $+ $(LIBS)

# We refer explicitly to SRCDIR/%.c so that the bison/flex output will be placed in SRCDIR
%.lo: $(SRCDIR)/%.c
	$(LIBTOOL) $(CC) -c $(CFLAGS) $<

scanner.c:: parser.h

%.y: %.ym grammer.y
	$(M4) -s -I$(SRCDIR) -P < $< > $@

%.c: %.y
	$(BISON) -v -d -o $@ $<

%.h: %.c
	@true

%.c: %.l 
	$(FLEX) -i -Pyysmacql_ -o$@ $<

reallyclean: clean
	rm -Rf scanner.c parser.c parser.h scan.c gram.c gram.h

clean:
	rm -Rf *.lo *.la *.o *.a .libs

