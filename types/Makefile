include config.mk

LDFLAGS=-lglib-2.0 -lgmodule-2.0 $(LDOPTS)

BUILTIN_TYPES=int ushort ubyte timeval uint32 bytes netlong ip nushort nuint32 macaddr string double refresh empty ipproto packet cflow time ntime
MODULE_TYPES=

LIBS_BUILTIN=$(patsubst %,lib%.la,$(BUILTIN_TYPES))
LIBS_MODULES=$(patsubst %,dts_%.la,$(MODULE_TYPES))

all: $(LIBS_MODULES) libdts-modules.la

reallyclean: clean

clean:
	rm -Rf *.o *.so *.a *.la *.lo .libs

# What we really want is a convenience library that we can link into binaries
# with a "--whole-archive" flag.  However, libtool doesn't do "--whole-archive"
# on convenience libraries, so we make libtool think this is a module that
# we can preopen
libdts-modules.la: $(LIBS_BUILTIN)
	$(LIBTOOL) $(CCLD) -static -rpath /CONVENIENCE -o $@ $+

lib%.al lib%.la: %.lo
	$(LIBTOOL) $(CCLD) -o $@ $(LDFLAGS) $<

lib%.a: %.lo
	$(LIBTOOL) $(CCLD) -o $@ $(LDFLAGS) $<

packet.lo packet.o: ../include/dts_packet.h
packet.o: packet.c
	$(CC) -c $(CFLAGS) $< 

libpacket-ethereal.la: packet-ethereal.lo
	$(LIBTOOL) $(CCLD) -o $@ $<

libpacket.la: packet.lo libpacket-ethereal.la 
	#$(LIBTOOL) $(CC) -rpath /CONVENIENCE -L$(TOPBINDIR)/contrib -L.  -lpacket-ethereal -o $@ $< 
	@set -x; if [ "$$WITH" = "ethereal" ]; then \
		PREFIX="$(TOPBINDIR)/contrib/"; \
	fi; \
	$(LIBTOOL) $(CCLD) -o $@ $${PREFIX}libpacket-ethereal.la -o $@ $< 

%.o: %.c
	$(CC) -c $(CFLAGS) $<

%.lo: %.c
	$(LIBTOOL) $(CC) -c $(CFLAGS) $<

dts_%.la: %.lo 
	$(LIBTOOL) $(CCLD) -module -rpath /usr/local/lib -o $@ $(LDFLAGS) -lc $<

