AUTOMAKE_VERSION=1.7

default: 
	misc/buildarch

debug:
	BUILDNAME=debug CONFIG="--enable-debug" misc/buildarch 

profile:
	BUILDNAME=profile CONFIG="--enable-profile" misc/buildarch 

small:
	BUILDNAME=small CONFIG="--enable-small" misc/buildarch

dist:
	misc/buildarch dist
	(set -ex; misc/buildarch dist; d=/tmp/$$$$; mkdir $$d; tar -C $$d -xzf build/*/smacq-*.tar.gz; cd $$d/smacq-*; ./configure; make check); echo "IT'S A KEEPER:" build/*/smacq-*.tar.gz

bootstrap:
	rm -Rf autom4te.cache config/[a-z]*
	mkdir -p config
	export PATH=/sw/bin:$$PATH; \
	libtoolize --copy --force; \
	aclocal-$(AUTOMAKE_VERSION) -I libgnu/m4; \
	autoheader; \
	automake-$(AUTOMAKE_VERSION) -a; \
	autoconf

cvsinterforce: cvsinter
		# Have to force timestamp update even if contents unchanged
	cvs commit -m 'update timestamp' -f stamp-h.in aclocal.m4 *.in `find * -maxdepth 2 -name \*.in` configure

cvsinter:
		# All the intermediate files that may change
	cvs commit -m 'intermediate file' *.in `find * -maxdepth 2 -name \*.in` aclocal.m4 configure stamp-h.in config doc/smacq.1* doc/smacqq.1* #libsmacq/parser.h libsmacq/*parser.[yc]pp libsmacq/scanner.cpp

%:
	misc/buildarch $@

smacq.iso: dist
	TOPSRCDIR=`pwd` misc/mkiso
