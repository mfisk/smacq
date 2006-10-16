SUBDIRS=substr patricia
noinst_LTLIBRARIES = libsmacq_modules.la libtest_dynamic.la 
EXTRA_LTLIBRARIES = libsmacq_python.la libsmacq_sql.la libsmacq_pcaplive.la libsmacq_fastbit.la libsmacq_distribute.la
SUFFIXES = .sc

libsmacq_modules_la_SOURCES = print.cpp pcapfile.cpp pcapwrite.cpp socket.cpp uniq.cpp count.cpp last.cpp top.cpp head.cpp filter.cpp msgtest.cpp entropy.cpp derivative.cpp encrypt.cpp pdf.cpp fifo.cpp delta.cpp tabularinput.cpp rusage.cpp groupby.cpp flowid.cpp sum.cpp fifodelay.cpp take.cpp project.cpp rename.cpp mask.cpp const.cpp ndjoin.cpp lor.cpp counter_loop.cpp dfa.cpp expr.cpp equals.cpp cflow.cpp clock.cpp disarm.cpp substr-main.cpp iplookup.cpp streamalias.cpp join.cpp changes.cpp histogram.cpp now.cpp div.cpp private.cpp uniqobj.cpp sort.cpp countobj.cpp file.cpp deskew.cpp

libsmacq_modules_la_DEPENDENCIES = substr/libsubstr.la patricia/libpatricia.la libsmacq_pcaplive.la @EXTRA_STATIC_MODULES@ 
libsmacq_modules_la_LIBADD = substr/libsubstr.la patricia/libpatricia.la libsmacq_pcaplive.la @EXTRA_STATIC_MODULES@ -lbz2 -lz -lsmacq
libsmacq_modules_la_LDFLAGS = $(AM_LDFLAGS) -rpath /usr/lib -static -L$(top_builddir)/libsmacq

libsmacq_distribute_la_SOURCES = distribute.cpp

libtest_dynamic_la_SOURCES = noop.cpp

.sc.c: 
	$(top_srcdir)/misc/sc2c $< > $@


#
# Per module link requirements
# 
libsmacq_python_la_SOURCES = python.cpp
libsmacq_python_la_LDFLAGS = $(AM_LDFLAGS) @LDFLAGS_PYTHON@ @LIBS_PYTHON@
libsmacq_python_la_CPPFLAGS = $(AM_CPPFLAGS) @CFLAGS_PYTHON@ 

libsmacq_sql_la_SOURCES = sqlinsert.cpp sqlquery.cpp
libsmacq_sql_la_LIBADD = @GDA_LIBS@
libsmacq_sql_la_CPPFLAGS = $(AM_CPPFLAGS) @GDA_CFLAGS@

libsmacq_pcaplive_la_SOURCES = pcaplive.cpp
libsmacq_pcaplive_la_LIBS = -lpcap

libsmacq_fastbit_la_SOURCES = fastbit.cpp
libsmacq_fastbit_la_LDFLAGS = $(AM_LDFLAGS) @LDFLAGS_FASTBIT@
libsmacq_fastbit_la_CPPFLAGS = $(AM_CPPFLAGS) @CFLAGS_FASTBIT@

#libbench_field_la_SOURCES = bench_field-main.c bench_field-virtual.C 

#libreassemble_la_LIBADD = reassemble.lo spp_export_pkt.lo  
#reassemble_lo_CFLAGS = -I$(SNORT_HOME)

libepan_filter.o: epan_filter.o 
	$(LD) -r -o $@ $< -L$(TOPBINDIR)/contrib -L. -lpacket-ethereal

#
# This is how we make a dynamic library for a module
# 
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
	$(CC) $(CFLAGS) -DHAVE_CONFIG_H -I$(SNORT_HOME) -c $<

