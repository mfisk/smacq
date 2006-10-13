noinst_LTLIBRARIES = libdts_modules.la libdts_packet.la
noinst_HEADERS = packet_epan.cpp

libdts_modules_la_SOURCES = bytes.cpp empty.cpp ipproto.cpp ntime.cpp string.cpp ubyte.cpp cflow.cpp int.cpp macaddr.cpp nuint32.cpp time.cpp uint32.cpp double.cpp ip.cpp netlong.cpp nushort.cpp refresh.cpp timeval.cpp ushort.cpp sv4.cpp uint64.cpp libdts_packet.la

libdts_modules_la_DEPENDENCIES = libdts_packet.la
libdts_modules_la_LIBADD = $(libdts_modules_la_DEPENDENCIES)
libdts_modules_la_LDFLAGS = $(AM_LDFLAGS) -rpath /usr/lib -static

libdts_packet_la_SOURCES = packet.cpp
libdts_packet_la_CPPFLAGS = $(AM_CPPFLAGS) @ETHEREAL_CPPFLAGS@ #-Ilibgnu
libdts_packet_la_LDFLAGS = @ETHEREAL_LDFLAGS@ 

