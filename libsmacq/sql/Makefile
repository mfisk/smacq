PGHOME = .

INCLUDES = -I. -I../../include \
	-I$(PGHOME)/include \
	-I$(PGHOME)/include/nodes \
	-I$(PGHOME)/include/tcop \
	-I$(PGHOME)/include/storage \
	-I$(PGHOME)/include/utils \
	-I$(PGHOME)/include/lib \
	-I$(PGHOME)/include/parser \
	-I$(PGHOME)/include/rewrite

OBJS = scan.lo gram.lo transform.lo\
  keywords.lo scansup.lo list.lo nodes.lo equalfuncs.lo datum.lo copyfuncs.lo

CFLAGS = -ggdb -Wall -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include $(INCLUDES) -DFRONTEND

LIBS = -lgmodule-2.0 -lglib-2.0 -lz -lcrypt -lresolv -lnsl -ldl -lm

all: libsql.la

gram.c:	gram.y scan.l
	-bison -t -y -d $< -o $@

gram.lo: gram.c
	$(LIBTOOL) $(CC) -c $(CFLAGS) $<

scan.c: scan.l gram.c 
#	flex -d -i -o$@ $<
	-flex -i -o$@ $<

scan.lo: scan.c
	$(LIBTOOL) $(CC) -c $(CFLAGS) $<

libsql.la: $(OBJS)
	$(LIBTOOL) $(CC) -o $@ $+

%.lo: %.c
	$(LIBTOOL) $(CC) -c $(CFLAGS) $<

clean: distclean
	rm -Rf scan.c gram.c gram.h

distclean:
	rm -Rf *.lo *.la *.o *.a .libs

