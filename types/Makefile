CFLAGS= -Winline -Wall -I$(SRCDIR) -I$(USR)/include/glib-2.0 -I$(USR)/lib/glib-2.0/include -I$(SRCDIR)/../include $(COPTS)
LDFLAGS=-lglib-2.0 -lgmodule-2.0 $(LDOPTS)

BUILTIN_TYPES=packet int ushort ubyte timeval uint32 bytes netlong ip nushort nuint32 macaddr string double refresh empty ipproto 
MODULE_TYPES=

TYPEMODULES=$(patsubst %,dts_%.la,$(MODULE_TYPES))
TYPEOBJS=$(patsubst %,%.lo,$(BUILTIN_TYPES))

all: $(TYPEMODULES) libdts-modules.la

reallyclean: clean

clean:
	rm -Rf *.o *.so *.a *.la *.lo .libs

# What we really want is a convenience library that we can link into binaries
# with a "--whole-archive" flag.  However, libtool doesn't do "--whole-archive"
# on convenience libraries, so we make libtool think this is a module that
# we can preopen
libdts-modules.la: $(TYPEOBJS)
	$(LIBTOOL) $(CC) -static -rpath /CONVENIENCE -o $@ $+

smacqtype_packet.so: ../include/dts_packet.h

%.lo: %.c 
	$(LIBTOOL) $(CC) -c $(CFLAGS) $<

dts_%.la: %.lo 
	$(LIBTOOL) $(CC) -module -rpath /usr/local/lib -o $@ $(LDFLAGS) -lc $<

libethereal.la: ethereal.c
	$(LIBTOOL) $(CC) -module -rpath /usr/local/lib -o $@ $(LDFLAGS) -lc $<
