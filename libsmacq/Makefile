
CFLAGS= -Winline -Wall -I$(SRCDIR) -I$(USR)/include/glib-2.0 -I$(USR)/lib/glib-2.0/include -I$(SRCDIR)/../include/ $(COPTS)
LDFLAGS=-lglib-2.0 -lgmodule-2.0 -lm $(LDOPTS)

OBJS=mem.lo bytehash.lo fields.lo bloom.lo args.lo pickle.lo types.lo sched-thread.lo sched-mono.lo sched-iter.lo construct.lo optimize.lo sched.lo msg.lo filter.lo module-thread.lo dynloader.lo parsing.lo sql/libsql.la filter/libfilter.la

DIRS=sql filter

all: dirs libsmacq.la 

include $(MAKEINCLUDES)

clean: 
	for f in $(DIRS); do $(MAKE) -C $$f clean; done
	rm -Rf *.o *.so *.a *.lo *.la .libs

reallyclean: 
	for f in $(DIRS); do $(MAKE) -C $$f reallyclean; done
	rm -Rf *.o *.so *.a *.lo *.la .libs

%.lo: %.c ../include/smacq.h ../include/smacq-internal.h ../include/types-inline.c
	$(LIBTOOL) $(CC) -c -o $@ $(CFLAGS) $<

libsmacq.la: $(OBJS)
	$(LIBTOOL) $(CC) -o $@ $+ $(LDFLAGS)

