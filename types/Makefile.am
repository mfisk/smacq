INCLUDES = @GLOBAL_INCLUDES@ -I$(top_srcdir)/include -I/usr/include/pcap

noinst_LTLIBRARIES = libdts_modules.la 
EXTRA_LTLIBRARIES = 

libdts_modules_la_SOURCES = bytes.c empty.c ipproto.c ntime.c string.c ubyte.c cflow.c int.c macaddr.c nuint32.c time.c uint32.c double.c ip.c netlong.c nushort.c refresh.c timeval.c ushort.c packet.c sv4.c uint64.c
libdts_modules_la_DEPENDENCIES = 
libdts_modules_la_LDFLAGS = -rpath /bogus

#dts_%.la: %.lo 
#	$(LINK) -module -rpath /usr/local/lib -o $@ -lc $<

