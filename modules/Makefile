#PCAPROUTINES=close compile datalink dump_close lookupnet loop open_live perror setfilter snapshot
#WRAPPCAPROUTINES=$(patsubst %,--wrap __real_pcap_%,$(PCAPROUTINES))

CFLAGS=-g -Wall -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I../include $(COPTS)

LDFLAGS=$(LDOPTS)
SO_LDFLAGS=-L../libsmacq -lsmacq -lpcap -lz -lglib-2.0 -lpython2.2 -lgmodule-2.0 -L. #-lsnort
STATIC_LDFLAGS=-L../contrib -L/usr/lib/python2.2/config -lpython2.2 -lpcap -lz

BUILTIN_MODULES=pcaplive pcapfile print socket uniq counter last top head intervals filter msgtest entropy derivative split encrypt warden pdf fifo delta tabularinput sync rusage groupby flowid sum fifodelay project

MODULES=$(BUILTIN_MODULES) #python #reassemble

MODULES_SHOBJS=$(patsubst %,smacq_%.la,$(MODULES))
MODULES_OBJS=$(patsubst %,%.lo,$(BUILTIN_MODULES))

# To separate our snort plugin code from the snort distribution
# (eg. put "spp_export_pkt.lo" in this list, and take it out of SNORT_OBJS)
#
LOCAL_SNORT_OBJS = spp_export_pkt.lo

all: $(MODULES_OBJS) $(MODULES_SHOBJS) libsmacq-modules.lo

SNORT_VERSION=snort-1.8.7
SNORT_HOME=../contrib/$(SNORT_VERSION)
SNORT_OBJS = $(SNORT_HOME)/*.lo

$(SNORT_HOME):
	make -C ../contrib $(SNORT_VERSION)

libsmacq-modules.lo: $(MODULES_OBJS) 
	$(LIBTOOL) $(CC) -o $@ $+ 

smacq-modules.off.a: modules-pass1.la
	#$(LD) -static -r -E -o $@ --whole-archive modules-pass1.a --no-whole-archive $(STATIC_LDFLAGS) 
	$(LD) -static -r -E -o $@ -all_load modules-pass1.a --no-whole-archive $(STATIC_LDFLAGS) 


clean: distclean

distclean:
	rm -Rf *.o *.so *.a *.lo *.la .libs

spp_export_pkt.lo: spp_export_pkt.c spp_export_pkt.h $(SNORT_HOME)
	$(CC) $(CFLAGS) -DHAVE_CONFIG_H -I$(SNORT_HOME) -I. -g -O2 -Wall -ggdb -c $<

reassemble.lo: reassemble.c spp_export_pkt.lo ../include/smacq.h ../libsmacq/libsmacq.a 
	$(CC) -o $@ -c $(CFLAGS) -I$(SNORT_HOME) -I. $< 

libsnort.la: ../contrib/snort-1.8.7/
	#ar -r $@ $<
	#ranlib $@
	$(LIBTOOL) $(CC) -static -o $@ $</[a-r]*.o
	$(LIBTOOL) $(CC) -dynamic -r -L/sw/lib -o $@ $</*.o -lpcap -lm

pcapfile.lo: pcapfile.c pcapfile.h 
pcaplive.lo: pcaplive.c pcapfile.h 

%.lo: %.c ../include/smacq.h 
	$(LIBTOOL) $(CC) -c $(CFLAGS) $< 

python.lo: python.c ../include/smacq.h 
	$(LIBTOOL) $(CC) -c $(CFLAGS) -I/usr/include/python2.2 $<

smacq_%.la: %.lo
	$(LIBTOOL) $(CC) -module -rpath /usr/local/lib $(LDFLAGS) -o $@ $+

