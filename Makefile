DIRS=libsmacq types modules bin doc #reloc
CFLAGS=-O9 -Winline 			# Optimized for normal use
CFLAGS=-ggdb -O0 -fno-inline -Winline	# For debugging
CFLAGS=-O0 -fno-inline -Winline		# MacOS/X doesn't like -ggdb 
COPTS+=$(CFLAGS)

auto:
	@./misc/config-env
	env `./misc/config-env` $(MAKE) all
	@echo "Executables are in build/":
	@ls -al build/*/bin/smacqq

smacq.iso: #reloc.RECURSE
	env `./misc/config-env` misc/mkiso

all: dirs

warn: 
	$(MAKE) auto >/dev/null

#
# Let the user build any top-level directory automatically
#
#test : .auto ALWAYS
	#env `./misc/config-env` $(MAKE) $@.RECURSE

test: auto

$(DIRS) test reloc : .ALWAYS
	env `./misc/config-env` $(MAKE) $@.RECURSE

.ALWAYS:
	@true

clean: 
	@for f in $(DIRS); do $(MAKE) -C $$f clean; done
	@rm -Rf build smacq.iso

reallyclean: 
	@for f in $(DIRS); do $(MAKE) -C $$f reallyclean; done

binclean:
	find . -type d -name .libs -o -name \*.lo -o -name \*.o -exec rm -Rf {} \;

dist: clean
	tar czplf flow.tgz flow/flow doc/*.pdf doc/*.txt doc/*.1 doc/*.3
	(cd /tmp; cvs -q -d cj:/home/cvs co flow); tar -C /tmp -czlf flow-src.tgz flow

	gnumake MAKE=gnumake COPTS="-I/sw/include/glib-2.0 -I/sw/lib/glib-2.0/include/ -I/sw/include" LIBTOOL=glibtool LDOPTS="-L/sw/lib" settings all

include misc/include.mk
