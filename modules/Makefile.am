SUBDIRS=substr patricia
INCLUDES = @GLOBAL_INCLUDES@ -I$(top_srcdir)/include -I/usr/include/pcap
noinst_LTLIBRARIES = libsmacq_modules.la libtest_dynamic.la 
EXTRA_LTLIBRARIES = #libpcaplive.la libpython.la libreassemble.la libbench_field.la 
BUILT_SOURCES = count.c
SUFFIXES = .sc

libsmacq_modules_la_SOURCES = print.c pcaplive.c pcapfile.c socket.c uniq.c count.c last.c top.c head.c filter.c msgtest.c entropy.c derivative.c split.c encrypt.c warden.c pdf.c fifo.c delta.c tabularinput.c sync.c rusage.c groupby.c flowid.c sum.c fifodelay.c project.c rename.c join.c mask.c const.c ndjoin.c lor.c counter_loop.c dfa.c noop.c expr.c equals.c cflow.c clock.c disarm.c substr-main.c iplookup.c #sqlinsert.c #sqlclient.c
EXTRA_libsmacq_modules_la_SOURCES = sqlinsert.c

libsmacq_modules_la_LIBADD = -lpcap substr/libsubstr.la patricia/libpatricia.la @EXTRA_MODULE_SOURCES@ 
libsmacq_modules_la_DEPENDENCIES = $(libsmacq_modules_la_LIBADD) @EXTRA_MODULE_SOURCES@ 

libtest_dynamic_la_SOURCES = test_dynamic.c

.sc.c: 
	$(top_srcdir)/misc/sc2c $< > $@

# What we really want is a convenience library that we can link into binaries
# with a "--whole-archive" flag.  However, libtool doesn't do "--whole-archive"
# on convenience libraries, so we make libtool think this is a module that
# we can preopen
libsmacq_modules_la_LDFLAGS = -rpath /bogus

#
# Per module link requirements
# 
#libbench_field_la_SOURCES = bench_field-main.c bench_field-virtual.C 

#libpcaplive_la_SOURCES = pcaplive.c
#libpcaplive_la_LDFLAGS = -lpcap

python_lo_CFLAGS = -I$(USR)/include/python2.2
#libpython_la_LIBADD = python.lo -L/usr/lib/python2.2/config -lpython2.2 -lpthread -lutil 

#libreassemble_la_LIBADD = reassemble.lo spp_export_pkt.lo  
reassemble_lo_CFLAGS = -I$(SNORT_HOME)

libepan_filter.o: epan_filter.o 
	$(LD) -r -o $@ $< -L$(TOPBINDIR)/contrib -L. -lpacket-ethereal

#
# This is how we make a dynamic library for a module
# 
RPATH=-rpath /usr/local/lib/smacq

smacq_%.la: lib%.la
	$(LIBTOOL) $(CCLD) -module $(LDFLAGS) -o $@ $+

#
# Extra Snort module stuff:
#
SNORT_VERSION=snort-1.8.7
SNORT_HOME=../contrib/$(SNORT_VERSION)

$(SNORT_HOME):
	$(MAKE) -C ../contrib $(SNORT_VERSION)

libsnort.la: $(SNORT_HOME)
	$(LIBTOOL) $(CCLD) -static -o $@ $</[a-r]*.o
	$(LIBTOOL) $(CCLD) -dynamic -r -L/sw/lib -o $@ $</*.o -lpcap -lm

spp_export_pkt.lo: spp_export_pkt.c spp_export_pkt.h $(SNORT_HOME)
	$(CC) $(CFLAGS) -DHAVE_CONFIG_H -I$(SNORT_HOME) -I. -g -O2 -Wall -ggdb -c $<

