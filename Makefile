DIRS=libsmacq contrib types modules bin doc #reloc
BUILDDIR=build/`uname -sm| sed 's/  */-/g'`
TOPSRCDIR=.

include config.mk

auto: 
	@misc/config-env
	@mkdir -p $(BUILDDIR)
	$(MAKE) -C $(BUILDDIR) -f `pwd`/Makefile dirs 
	@echo "Executables are in build/":
	@ls -al build/*/bin/smacqq
	ln -fs build/*/bin/smacqq smacqq

smacq.iso: #reloc.RECURSE
	./misc/mkiso

warn: 
	$(MAKE) auto >/dev/null

#
# Let the user build any top-level directory automatically
#
#test : .auto ALWAYS
	#env `./misc/config-env` $(MAKE) $@.RECURSE

test: auto

$(DIRS) test reloc : .ALWAYS
	$(MAKE) $@.RECURSE

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

include $(TOPSRCDIR)/misc/include.mk

