include config.mk

LDFLAGS=-lglib-2.0 -lgmodule-2.0 $(LDOPTS)

BUILTIN_TYPES=int ushort ubyte timeval uint32 bytes netlong ip nushort nuint32 macaddr string double refresh empty ipproto packet
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
	$(LIBTOOL) $(CC) -static -rpath /CONVENIENCE -o $@ $+

lib%.la: %.lo
	$(LIBTOOL) $(CC) -o $@ $(LDFLAGS) $<

lib%.a: %.lo
	$(LIBTOOL) $(CC) -o $@ $(LDFLAGS) $<

packet.o: ../include/dts_packet.h
packet.o: packet.c
	$(CC) -c $(CFLAGS) $< 

packet-ethereal.o: packet.o libpacket-ethereal.a  #$(TOPSRCDIR)/contrib/libethereal.a
	$(LD) -r -o $@ $< -L$(TOPBINDIR)/contrib -L. -lpacket-ethereal

#%-weak.o: %.o
#	objcopy --weaken $< $@

#libpacket.la: packet-ethereal-weak.o 
#	$(LIBTOOL) $(CC) -static -o $@ $< -lpcap

libpacket.la: packet-ethereal.o 
	$(LIBTOOL) $(CC) -static -o $@ $< #-L. -L$(TOPSRCDIR)/contrib -lpacket-ethereal
	#$(LIBTOOL) $(CC) -static -o $@ $< libpacket-ethereal.a

%.o: %.c
	$(CC) -c $(CFLAGS) $<

%.lo: %.c
	$(LIBTOOL) $(CC) -c $(CFLAGS) $<

dts_%.la: %.lo 
	$(LIBTOOL) $(CC) -module -rpath /usr/local/lib -o $@ $(LDFLAGS) -lc $<

