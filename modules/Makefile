#PCAPROUTINES=close compile datalink dump_close lookupnet loop open_live perror setfilter snapshot
#WRAPPCAPROUTINES=$(patsubst %,--wrap __real_pcap_%,$(PCAPROUTINES))

CFLAGS=-g -Wall -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I../include $(COPTS)

SO_LDFLAGS=-L../libsmacq -lsmacq -lpcap -lz -lglib-2.0 -lpython2.2 -lgmodule-2.0
STATIC_LDFLAGS=-L../contrib -L/usr/lib/python2.2/config -lpython2.2 -lpcap -lz

BUILTIN_MODULES=pcaplive pcapfile print socket uniq counter last top head intervals filter msgtest entropy derivative split encrypt warden pdf fifo delta tabularinput sync rusage groupby flowid
MODULES=$(BUILTIN_MODULES) reassemble python

MODULES_SHOBJS=$(patsubst %,smacq_%.so,$(MODULES))
MODULES_OBJS=$(patsubst %,%.o,$(BUILTIN_MODULES))

# To separate our snort plugin code from the snort distribution
# (eg. put "spp_export_pkt.o" in this list, and take it out of SNORT_OBJS)
#
LOCAL_SNORT_OBJS = spp_export_pkt.o

all: $(MODULES_OBJS) $(MODULES_SHOBJS) pcapfile-bsd.o  smacq-modules.a

SNORT_VERSION=snort-1.8.7
SNORT_HOME=../contrib/$(SNORT_VERSION)
SNORT_OBJS = $(SNORT_HOME)/*.o

$(SNORT_HOME):
	make -C ../contrib $(SNORT_VERSION)

modules-pass1.a: $(MODULES_OBJS) pcapfile-bsd.o
	ar -rc $@ $+ 

smacq-modules.a: modules-pass1.a
	$(LD) -static -r -E -o $@ --whole-archive modules-pass1.a --no-whole-archive $(STATIC_LDFLAGS) 


clean: 
	rm -f *.o *.so *.a 

spp_export_pkt.o: spp_export_pkt.c spp_export_pkt.h $(SNORT_HOME)
	$(CC) -DHAVE_CONFIG_H -I$(SNORT_HOME) -I. -g -O2 -Wall -ggdb -c $<
#	$(CC) -DHAVE_CONFIG_H -I$(SNORT_HOME) -I. -g -O2 -Wall -c $<

reassemble-pass1.o: reassemble.c spp_export_pkt.o ../include/smacq.h ../libsmacq/libsmacq.a 
	$(CC) -o $@ -c $(CFLAGS) -I$(SNORT_HOME) -I. $<

reassemble-pass2.o: reassemble-pass1.o
	$(LD) -r -o $@ $< $(LOCAL_SNORT_OBJS) $(SNORT_OBJS)

reassemble.o: reassemble-pass2.o
	objcopy --strip-symbol=main $< $@ 

pcapfile.o: pcapfile.c ../include/dts_packet.h 
pcaplive.o: pcaplive.c ../include/dts_packet.h 
smacq_pcapfile.so: pcapfile-bsd.o
smacq_pcaplive.so: pcapfile-bsd.o

%.o: %.c ../include/smacq.h 
	$(CC) -c $(CFLAGS) $< 

python.o: python.c ../include/smacq.h 
	$(CC) -c $(CFLAGS) -I/usr/include/python2.2 $<

smacq_%.so: %.o
	$(CC) $(LDFLAGS) -shared -o $@ $+ $(SO_LDFLAGS)



