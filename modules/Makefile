DIRS=substr #Must come before config.mk

include config.mk

LIBSMACQ=-L$(TOPBINDIR)/libsmacq -lsmacq
LDFLAGS=$(LDOPTS) #$(LIBSMACQ)

SHAREDONLY_MODULES=test_dynamic #python #reassemble
BUILTIN_MODULES=print pcaplive pcapfile socket uniq count last top head intervals filter msgtest entropy derivative split encrypt warden pdf fifo delta tabularinput sync rusage groupby flowid sum fifodelay project rename join substr bench_field mask const ndjoin lor counter_loop dfa noop expr equals cflow clock

LIBS_BUILTIN=$(patsubst %,lib%.la,$(BUILTIN_MODULES)) 
LIBS_SHARED=$(patsubst %,smacq_%.la,$(SHAREDONLY_MODULES))

all: dirs $(LIBS_SHARED) libsmacq-modules.la

reallyclean: clean
clean:
	rm -Rf *.o *.so *.a *.lo *.la .libs
	@set -e; for f in $(DIRS); do $(MAKE) -C $$f clean; done

#
# Per module link requirements
# 
libsubstr.la: substr-main.lo $(BINDIR)/substr/libsubstr.la
	$(LIBTOOL) $(CC) -o $@ $(LDFLAGS) $+

libbench_field.la: bench_field-main.lo bench_field-virtual.lo 
	$(LIBTOOL) $(CC) -o $@ $(LDFLAGS) $+

lib%.la: %.lo
	$(LIBTOOL) $(CC) -o $@ $(LDFLAGS) $+

libpcapfile.la: pcapfile.lo
	$(LIBTOOL) $(CC) -o $@ $(LDFLAGS) $+ -lz -lpcap

libpcaplive.la: pcaplive.lo
	$(LIBTOOL) $(CC) -o $@ $(LDFLAGS) $+ -lpcap

libpython.la: python.lo
	$(LIBTOOL) $(CC) -o $@ $(LDFLAGS) $+ -L/usr/lib/python2.2/config -lpython2.2 -lpthread -lutil 

libreassemble.la: reassemble.lo spp_export_pkt.lo  
	$(CC) -o $@ -c $(CFLAGS) -I$(SNORT_HOME) -I. $+ 

libepan_filter.o: epan_filter.o 
	$(LD) -r -o $@ $< -L$(TOPBINDIR)/contrib -L. -lpacket-ethereal

#
# Per module compile info
# 
%.lo: %.c $(TOPSRCDIR)/include/smacq.h
	$(LIBTOOL) $(CC) -c $(CFLAGS) $< 

%.lo: %.C $(TOPSRCDIR)/include/smacq.h 
	$(LIBTOOL) $(CXX) -c $(CFLAGS) $< 

python.lo: python.c $(TOPSRCDIR)/include/smacq.h 
	$(LIBTOOL) $(CC) -c $(CFLAGS) -I$(USR)/include/python2.2 $<

# What we really want is a convenience library that we can link into binaries
# with a "--whole-archive" flag.  However, libtool doesn't do "--whole-archive"
# on convenience libraries, so we make libtool think this is a module that
# we can preopen

libsmacq-modules.la: $(LIBS_BUILTIN) 
	$(LIBTOOL) $(CC) -static -rpath /CONVIENCE -o $@ $+

#
# This is how we make a dynamic library for a module
# 
RPATH=-rpath /usr/local/lib/smacq

smacq_%.la: lib%.la
	$(LIBTOOL) $(CC) -module $(LDFLAGS) -o $@ $+

#
# Extra Snort module stuff:
#
SNORT_VERSION=snort-1.8.7
SNORT_HOME=../contrib/$(SNORT_VERSION)

$(SNORT_HOME):
	$(MAKE) -C ../contrib $(SNORT_VERSION)

libsnort.la: $(SNORT_HOME)
	$(LIBTOOL) $(CC) -static -o $@ $</[a-r]*.o
	$(LIBTOOL) $(CC) -dynamic -r -L/sw/lib -o $@ $</*.o -lpcap -lm

spp_export_pkt.lo: spp_export_pkt.c spp_export_pkt.h $(SNORT_HOME)
	$(CC) $(CFLAGS) -DHAVE_CONFIG_H -I$(SNORT_HOME) -I. -g -O2 -Wall -ggdb -c $<

