#
# This file is included by all Makefiles.
#
CFLAGS=-O9 -Winline 			# Optimized for normal use
CFLAGS=-ggdb -O0 -fno-inline -Winline	# For debugging
CFLAGS=-O0 -fno-inline -Winline		# MacOS/X doesn't like -ggdb 

CFLAGS+=-I$(USR)/include/glib-2.0 -I$(USR)/lib/glib-2.0/include -I$(TOPSRCDIR)/include -I$(SRCDIR)
CFLAGS+=$(COPTS)
