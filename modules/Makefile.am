SUBDIRS=substr patricia
INCLUDES = @GLOBAL_INCLUDES@ -I$(top_srcdir)/include -I/usr/include/pcap
noinst_LTLIBRARIES = libsmacq_modules.la libtest_dynamic.la 
EXTRA_LTLIBRARIES = #libpcaplive.la libreassemble.la libbench_field.la 
#BUILT_SOURCES = count.cpp
SUFFIXES = .sc

libsmacq_modules_la_SOURCES = print.cpp pcaplive.cpp pcapfile.cpp socket.cpp uniq.cpp count.cpp last.cpp top.cpp head.cpp filter.cpp msgtest.cpp entropy.cpp derivative.cpp encrypt.cpp pdf.cpp fifo.cpp delta.cpp tabularinput.cpp rusage.cpp groupby.cpp flowid.cpp sum.cpp fifodelay.cpp project.cpp rename.cpp mask.cpp const.cpp ndjoin.cpp lor.cpp counter_loop.cpp dfa.cpp expr.cpp equals.cpp cflow.cpp clock.cpp disarm.cpp substr-main.cpp iplookup.cpp

EXTRA_libsmacq_modules_la_SOURCES = sqlinsert.cpp python.cpp


libsmacq_modules_la_LIBADD = -lpcap substr/libsubstr.la patricia/libpatricia.la @SQLINSERT_LIBADD@ @PYTHON_LIBADD@
libsmacq_modules_la_DEPENDENCIES = substr/libsubstr.la patricia/libpatricia.la @SQLINSERT_DEPENDENCIES@ @PYTHON_DEPENDENCIES@

libtest_dynamic_la_SOURCES = noop.cpp

python.lo: python.cpp
	$(LTCXXCOMPILE) @CFLAGS_PYTHON@ -c $<

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

