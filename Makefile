DIRS=libsmacq types modules bin doc
CFLAGS=-O9 -Winline 			# Optimized for normal use
CFLAGS=-ggdb -O0 -fno-inline -Winline	# For debugging

auto:
	env `./config-env` make all

portabletar: auto
	@LIBS=`ldd bin/smacqq | cut -d'>' -f2 | cut -d'(' -f1`; \
	rm -Rf smacq; \
	mkdir -p smacq/bin smacq/lib; \
	cp bin/smacqq smacq/bin/; \
	cp bin/smacqq.sh smacq/smacqq; \
	cp $$LIBS smacq/lib/
	
all: dirs

dirs: 
	@set -e; for f in $(DIRS); do \
		LIBTOOL="$(LIBTOOL)" \
		COPTS="$(COPTS) $(CFLAGS)" \
		$(MAKE) -C $$f; \
	done

warn: 
	make auto >/dev/null

test: warn
	$(MAKE) -C test

clean: 
	@for f in $(DIRS); do $(MAKE) -C $$f clean; done

reallyclean: 
	@for f in $(DIRS); do $(MAKE) -C $$f reallyclean; done

binclean:
	find . -type d -name .libs -o -name \*.lo -o -name \*.o -exec rm -Rf {} \;

dist: clean
	tar czplf flow.tgz flow/flow doc/*.pdf doc/*.txt doc/*.1 doc/*.3
	(cd /tmp; cvs -q -d cj:/home/cvs co flow); tar -C /tmp -czlf flow-src.tgz flow

	gnumake MAKE=gnumake COPTS="-I/sw/include/glib-2.0 -I/sw/lib/glib-2.0/include/ -I/sw/include" LIBTOOL=glibtool LDOPTS="-L/sw/lib" settings all
fink:
