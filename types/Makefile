CFLAGS=-g -O3 -Winline -Wall -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I../include $(COPTS)
LDFLAGS=-lglib-2.0 -lgmodule-2.0 $(LDOPTS)

TYPES=packet int ushort ubyte timeval uint32 bytes netlong ip nushort nuint32 macaddr string double refresh empty
TYPEMODULES=$(patsubst %,dts_%.la,$(TYPES))
TYPEOBJS=$(patsubst %,%.lo,$(TYPES))

all: $(TYPEMODULES) libdts-modules.lo

reallyclean: clean

clean:
	rm -Rf *.o *.so *.a *.la *.lo .libs

libdts-modules.lo: $(TYPEOBJS)
	$(LIBTOOL) $(CC) -o $@ $+

smacqtype_packet.so: ../include/dts_packet.h

%.lo: %.c 
	$(LIBTOOL) $(CC) -c $(CFLAGS) $<

dts_%.la: %.lo 
	$(LIBTOOL) $(CC) -module -rpath /usr/local/lib -o $@ $(LDFLAGS) -lc $<
