#
# This file is included by all Makefiles.
#
CFLAGS=-g -O0 -fno-inline 	# MacOS/X doesn't like -ggdb 
CFLAGS=-ggdb -O0 -fno-inline 	# For debugging
CFLAGS=-O9  			# Optimized for normal use

CFLAGS+=-Winline -Wall -I$(USR)/include/glib-2.0 -I$(USR)/lib/glib-2.0/include -I$(TOPSRCDIR)/include -I$(SRCDIR)

#
# Debugging options
# 
CFLAGS+=-DSMACQ_DEBUG
#CFLAGS+=-DSMACQ_DEBUG_MEM

#
# All optimizations are on by default.
# To change that, use NO_OPT and enable optimizations individually
# 
#CFLAGS+= -DSMACQ_NO_OPT_DTS
#CFLAGS+= 	-DSMACQ_OPT_NOPTHREADS
#CFLAGS+= 	-DSMACQ_OPT_NOMSGS
#CFLAGS+= 	-DSMACQ_OPT_FORCEFIELDCACHE
#CFLAGS+= 	-DSMACQ_OPT_RUNRING

#CFLAGS+= -DSMACQ_NO_OPT_DATAFLOW
#CFLAGS+= 	-DSMACQ_OPT_VECTORS
#CFLAGS+= 	-DSMACQ_OPT_CHILDREN
#CFLAGS+= 	-DSMACQ_OPT_HEADS
#CFLAGS+= 	-DSMACQ_OPT_TAILS

CFLAGS+=$(COPTS)
