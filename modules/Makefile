DIRS=substr
CFLAGS=-Wall -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I../include $(COPTS)
LDFLAGS=$(LDOPTS)
LIBTOOL=libtool

BUILTIN_MODULES=pcaplive pcapfile print socket uniq counter last top head intervals filter msgtest entropy derivative split encrypt warden pdf fifo delta tabularinput sync rusage groupby flowid sum fifodelay project rename join substr bench_field mask const

MODULES=$(BUILTIN_MODULES) #python #reassemble

MODULES_SHOBJS=$(patsubst %,smacq_%.la,$(MODULES))
MODULES_OBJS=$(patsubst %,%.lo,$(BUILTIN_MODULES))

all: dirs $(MODULES_OBJS) $(MODULES_SHOBJS) libsmacq-modules.lo

dirs: 
	@set -e; for f in $(DIRS); do $(MAKE) -C $$f; done
	
reallyclean: clean
clean:
	rm -Rf *.o *.so *.a *.lo *.la .libs
	@set -e; for f in $(DIRS); do $(MAKE) -C $$f clean; done

pcapfile.lo: pcapfile.c pcapfile.h 
pcaplive.lo: pcaplive.c pcapfile.h 
substr.lo: substr/libsubstr.la

substr.lo: substr-main.lo ../include/smacq.h substr/libsubstr.la
	$(LIBTOOL) $(CC) -o $@ $(CFLAGS) $< substr/libsubstr.la 

bench_field.lo: bench_field-main.lo bench_field-virtual.lo ../include/smacq.h substr/libsubstr.la
	$(LIBTOOL) $(CC) -o $@ $(CFLAGS) $< bench_field-virtual.lo

libsmacq-modules.lo: $(MODULES_OBJS) 
	$(LIBTOOL) $(CC) -o $@ $+ 

smacq_%.la: %.lo
	$(LIBTOOL) $(CC) -module -rpath /usr/local/lib $(LDFLAGS) -o $@ $+

%.lo: %.c ../include/smacq.h 
	$(LIBTOOL) $(CC) -c $(CFLAGS) $< 

%.lo: %.C ../include/smacq.h 
	$(LIBTOOL) c++ -c $(CFLAGS) $< 

#
# Python module stuff:
#
python.lo: python.c ../include/smacq.h 
	$(LIBTOOL) $(CC) -o $@ $(CFLAGS) -I/usr/include/python2.2 $<

#
# Snort module stuff:
#
SNORT_VERSION=snort-1.8.7
SNORT_HOME=../contrib/$(SNORT_VERSION)

$(SNORT_HOME):
	make -C ../contrib $(SNORT_VERSION)

libsnort.la: ../contrib/snort-1.8.7/
	$(LIBTOOL) $(CC) -static -o $@ $</[a-r]*.o
	$(LIBTOOL) $(CC) -dynamic -r -L/sw/lib -o $@ $</*.o -lpcap -lm

spp_export_pkt.lo: spp_export_pkt.c spp_export_pkt.h $(SNORT_HOME)
	$(CC) $(CFLAGS) -DHAVE_CONFIG_H -I$(SNORT_HOME) -I. -g -O2 -Wall -ggdb -c $<

reassemble.lo: reassemble.c spp_export_pkt.lo ../include/smacq.h ../libsmacq/libsmacq.a 
	$(CC) -o $@ -c $(CFLAGS) -I$(SNORT_HOME) -I. $< 


