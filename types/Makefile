CFLAGS=-g -O3 -Winline -Wall -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I../include $(COPTS)
LDFLAGS=-lglib-2.0 -lgmodule-2.0

TYPES=packet int ushort ubyte timeval uint32 bytes netlong ip nushort nuint32 macaddr string double refresh empty
TYPEMODULES=$(patsubst %,dts_%.so,$(TYPES))
TYPEOBJS=$(patsubst %,%.o,$(TYPES))

all: $(TYPEMODULES) dts-modules.a

clean: 
	rm -f *.o *.so *.a

dts-modules.a: $(TYPEOBJS)
	ar -rc $@ $+

flowtype_packet.so: ../include/dts_packet.h

%.o: %.c 
	$(CC) -c $(CFLAGS) $<

dts_%.so: %.c 
	$(CC) -shared -o $@ $(CFLAGS) $<
