PCAPROUTINES=close compile datalink dump_close lookupnet loop open_live perror setfilter snapshot
CCWRAP=$(patsubst %,-Xlinker --wrap -Xlinker pcap_%,$(PCAPROUTINES))
LDWRAP=$(patsubst %,--wrap pcap_%,$(PCAPROUTINES))

CFLAGS=-g -O3 -Winline -Wall -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I../include
LDFLAGS=-L/usr/lib/python2.2/config

all: libpcap-preload.so

libpcap.a: runtime.a pcap-replacement.o 
	ld -E -o $@ -r --whole-archive $(LDWRAP) $+ $(LDFLAGS) --no-whole-archive -ldl -lpython2.2 -lglib-2.0

runtime.a: ../types/dts-modules.a ../libsmacq/libsmacq.a
	ld -E -o $@ -r -static --whole-archive $+ -lpcap --no-whole-archive -lglib-2.0 -lz -lm -lpthread -ldl -lutil -lgmodule-2.0

libpcap.so: libpcap.a
	$(CC) -o $@ -shared $(CCWRAP) $< 

libpcap.so.0: libpcap.so
	ln -s $< $@

libpcap-preload.so: pcap-preload.o ../libsmacq/libsmacq.a ../types/dts-modules.a ../modules/smacq-modules.a ../modules/pcapfile.o
	$(LD) -shared -o $@ $< ../modules/pcapfile.o --whole-archive ../types/dts-modules.a --no-whole-archive ../modules/pcapfile-bsd.o ../libsmacq/libsmacq.a -lglib-2.0 -ldl -lgmodule-2.0 -lpthread -lz

../modules/pcapfile.o:
	$(MAKE) -C ../modules pcapfile.o

%.o: %.c
	$(CC) -c $(CFLAGS) $<

clean: distclean

distclean:
	rm -Rf *.o *.a *.so .libs

