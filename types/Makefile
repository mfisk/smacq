CFLAGS= -Winline -Wall -I$(USR)/include/glib-2.0 -I$(USR)/lib/glib-2.0/include -I../include $(COPTS)
LDFLAGS=-lglib-2.0 -lgmodule-2.0 $(LDOPTS)

TYPES=packet int ushort ubyte timeval uint32 bytes netlong ip nushort nuint32 macaddr string double refresh empty ipproto 
TYPEMODULES=$(patsubst %,dts_%.la,$(TYPES))
TYPEOBJS=$(patsubst %,%.lo,$(TYPES))

all: $(TYPEMODULES) libdts-modules.la

reallyclean: clean

clean:
	rm -Rf *.o *.so *.a *.la *.lo .libs

libdts-modules.la: $(TYPEOBJS)
	$(LIBTOOL) $(CC) -o $@ -static $+

smacqtype_packet.so: ../include/dts_packet.h

%.lo: %.c 
	$(LIBTOOL) $(CC) -c $(CFLAGS) $<

dts_%.la: %.lo 
	$(LIBTOOL) $(CC) -module -rpath /usr/local/lib -o $@ $(LDFLAGS) -lc $<

libethereal.la: ethereal.c
	$(LIBTOOL) $(CC) -module -rpath /usr/local/lib -o $@ $(LDFLAGS) -lc $<
