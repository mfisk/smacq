CFLAGS=-g -O3 -Winline -Wall -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I../include/ $(COPTS)
LDFLAGS=-lglib-2.0 -lgmodule-2.0

DIRS=sql

all: dirs libsmacq.a libsmacq.so #libflow-static.a #libflow-engine.a libflow-engine-static.a #libflow.so libflow-engine.so

dirs: 
	@for f in $(DIRS); do $(MAKE) -C $$f; done

USEROBJS=libsmacq.o bytehash.o fields.o bloom.o args.o pickle.o types.o sched-thread.o sched-mono.o sched-iter.o construct.o sched.o msg.o filter.o module-thread.o sql/libsql.o
ENGINEOBJS=types.o construct.o $(USEROBJS)

SQLOBJS=sql/*.o
$(SQLOBJS): dirs

clean: 
	for f in $(DIRS); do $(MAKE) -C $$f clean; done
	rm -f *.o *.so *.a 

libsmacq-engine.o: libsmacq.c ../include/smacq.h ../include/flow-internal.h
	$(CC) -c -o $@ $(CFLAGS) $< 

libsmacq.o: libsmacq.c ../include/smacq.h ../include/flow-internal.h
	$(CC) -c -o $@ $(CFLAGS) $< 

types.o: types.c ../include/smacq.h ../include/flow-internal.h
	$(CC) -c -o $@ $(CFLAGS) $< 

libsmacq-engine.so: $(ENGINEOBJS)
	$(CC) -shared -o $@ $(CFLAGS) $+ -lglib-2.0 -lm

libsmacq-engine.a: $(ENGINEOBJS)
	ar -rc $@ $+

libsmacq.so: $(USEROBJS)
	$(CC) -shared -o $@ $(CFLAGS) $+ -lglib-2.0 -lm

#libsmacq.a: $(USEROBJS)
libsmacq.a: $(USEROBJS)
	ar -rc $@ $+

%-static.a: %.a
	cp $< $@

