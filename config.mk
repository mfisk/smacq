#
# This file is included by all Makefiles.
#
CFLAGS=-O9  			# Optimized for normal use
CFLAGS=-ggdb -O0 -fno-inline 	# For debugging
CFLAGS=-O0 -fno-inline 		# MacOS/X doesn't like -ggdb 

CFLAGS+=-Winline -I$(USR)/include/glib-2.0 -I$(USR)/lib/glib-2.0/include -I$(TOPSRCDIR)/include -I$(SRCDIR)
CFLAGS+=$(COPTS)
