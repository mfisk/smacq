#
# This file is included by all Makefiles.
#
CFLAGS=-g -O0 -fno-inline 	# MacOS/X doesn't like -ggdb 
CFLAGS=-ggdb -O0 -fno-inline 	# For debugging
CFLAGS=-O9  			# Optimized for normal use

CFLAGS+=-Winline -Wall -I$(USR)/include/glib-2.0 -I$(USR)/lib/glib-2.0/include -I$(TOPSRCDIR)/include -I$(SRCDIR)
CXXFLAGS=-finline-limit=100000
CFLAGS+=$(COPTS)
