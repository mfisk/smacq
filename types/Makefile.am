#LDFLAGS = -lglib-2.0 -lgmodule-2.0 $(LDOPTS)
INCLUDES = @GLOBAL_INCLUDES@ -I$(top_srcdir)/include

noinst_LTLIBRARIES = libdts_modules.la libdts_modules_pass1.la #@LIBS_TYPES_MODULES@
EXTRA_LTLIBRARIES = libpacket_la #libpacket_ethereal_la

libdts_modules_pass1_la_SOURCES = bytes.c empty.c ipproto.c ntime.c string.c ubyte.c cflow.c int.c macaddr.c nuint32.c packet.c time.c uint32.c double.c ip.c netlong.c nushort.c refresh.c timeval.c ushort.c packet-ethereal.c 
#int.c ushort.c packet.c ubyte.c @SOURCES_TYPES_BUILTIN@

libdts_modules_la_SOURCES =
libdts_modules_la_LIBADD = libdts_modules_pass1.la
libdts_modules_la_DEPENDENCIES = libdts_modules_pass1.la
libdts_modules_la_LDFLAGS = -rpath /bogus

#dts_%.la: %.lo 
#	$(LINK) -module -rpath /usr/local/lib -o $@ -lc $<

