DIRS=libsmacq types modules bin doc #reloc
CFLAGS=-O9 -Winline 			# Optimized for normal use
CFLAGS=-ggdb -O0 -fno-inline -Winline	# For debugging
CFLAGS=-O0 -fno-inline -Winline		# MacOS/X doesn't like -ggdb 
COPTS+=$(CFLAGS)

auto:
	@./misc/config-env
	env `./misc/config-env` $(MAKE) all

smacq.iso: #reloc.RECURSE
	rm -Rf build/iso
	mkdir -p build/iso/doc/
	mkdir -p build/iso/.bin/

	cp misc/smacq.ico misc/autorun.inf build/iso/
	cp doc/*.pdf build/iso/doc/
	cp build/Linux-i686/bin/smacqq misc/reloc build/iso/.bin
	(cd build/iso; ln -f .bin/reloc smacqq)

	mkdir -p build/iso/SMACQ.app #For MacOS

	# -hidden removes from Unix and requires /A on DOS, but shows on MacOS
	# -hide-joliet only hides from DOS/MacOSX
	mkisofs -hfs -hide-joliet '.*' -hide-hfs autorun.inf -hidden 'autorun.inf' -hide-hfs '*.ico' -hide-hfs 'smacqq' -hidden SMACQ.app -hidden '*.ico' -V SMACQ -hfs-volid "SMACQ for Mac" --osx-double -R build/iso > $@
	
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
