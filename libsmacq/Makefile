CFLAGS=-g -O3 -Winline -Wall -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I../include/ $(COPTS)
LDFLAGS=-lglib-2.0 -lgmodule-2.0 $(LDOPTS)

DIRS=sql filter

all: dirs libsmacq.la #libflow-static.a #libflow-engine.a libflow-engine-static.a #libflow.so libflow-engine.so

dirs: 
	@set -e; for f in $(DIRS); do $(MAKE) -C $$f; done

USEROBJS=libsmacq.lo bytehash.lo fields.lo bloom.lo args.lo pickle.lo types.lo sched-thread.lo sched-mono.lo sched-iter.lo construct.lo sched.lo msg.lo filter.lo module-thread.lo dynloader.lo sql/libsql.la filter/libfilter.la

ENGINEOBJS=types.lo construct.lo $(USEROBJS)

clean: 
	for f in $(DIRS); do $(MAKE) -C $$f clean; done
	rm -Rf *.o *.so *.a *.lo *.la .libs

reallyclean: 
	for f in $(DIRS); do $(MAKE) -C $$f reallyclean; done
	rm -Rf *.o *.so *.a *.lo *.la .libs

%.lo: %.c
	$(LIBTOOL) $(CC) -c $(CFLAGS) $<

libsmacq-engine.lo: libsmacq.c ../include/smacq.h ../include/smacq-internal.h
	$(LIBTOOL) $(CC) -c -o $@ $(CFLAGS) $< 

libsmacq.lo: libsmacq.c ../include/smacq.h ../include/smacq-internal.h
	$(LIBTOOL) $(CC) -c -o $@ $(CFLAGS) $< 

types.lo: types.c ../include/smacq.h ../include/smacq-internal.h
	$(LIBTOOL) $(CC) -c -o $@ $(CFLAGS) $< 

libsmacq-engine.la: $(ENGINEOBJS)
	$(LIBTOOL) $(CC) -o $@ $(CFLAGS) $+ -lglib-2.0 -lm

libsmacq-engine.a: $(ENGINEOBJS)
	$(LIBTOOL) $(CC) -o $@ $+

libsmacq.la: $(USEROBJS)
	$(LIBTOOL) $(CC) -rpath /usr/local/lib -o $@ $+ 

%-static.la: %.la
	cp $< $@

