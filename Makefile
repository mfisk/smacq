WMAKE=$(MAKE) >&-
DIRS=libsmacq types modules bin doc
OS=`uname -s`
CFLAGS=-O9 -Winline 			# Optimized for normal use
CFLAGS=-ggdb -O0 -fno-inline -Winline	# For debugging

auto:
	LIBTOOL=libtool; \
	MAKE=$(MAKE); \
	if [ `uname -s` == "Darwin" ]; then \
		MAKE=gnumake; \
		COPTS="-I/sw/include/glib-2.0 -I/sw/lib/glib-2.0/include/ -I/sw/include"; \
		LIBTOOL=glibtool; \
		LDOPTS="-L/sw/lib"; \
	else \
		if which g++296 > /dev/null; then \
			CXX="--mode=compile g++296"; \
		fi ;\
		if which gcc296 > /dev/null; then \
			CC=gcc296; \
		fi ;\
	fi ;\
	export MAKE COPTS LIBTOOL LDOPTS CC CXX ;\
	$$MAKE settings all

all: dirs

dirs: 
	@set -e; for f in $(DIRS); do \
		LIBTOOL="$(LIBTOOL)" \
		COPTS="$(COPTS) $(CFLAGS)" \
		$(MAKE) -C $$f; \
	done

warn: 
	make auto >/dev/null

settings:
	@echo "CC=$$CC"; \
	echo "CXX=$$CXXC"; \
	echo "LIBTOOL=$$LIBTOOL"; \
	echo "LDOPTS=$$LDOPTS"; \
	echo "COPTS=$$COPTS"; \
	echo "CFLAGS=$$CFLAGS"; \
	echo "MAKE=$$MAKE" 

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
