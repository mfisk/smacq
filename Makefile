#AUTOMAKE_VERSION=-1.9
#AUTOCONF_VERSION=2.59

# We don't make this the default in distributions so that we're more robust 
# to future gcc pedanticism, but for developers (who run this Makefile), 
# be as pedantic as possible.
CONFIG=--enable-pedantic

default: config/config.guess
	MAKEFLAGS="$(MAKEFLAGS)" CONFIG="$(CONFIG)" misc/buildarch

config/config.guess: 
	$(MAKE) $(MAKEFLAGS) bootstrap

debug:
	+MAKEFLAGS="$(MAKEFLAGS)" BUILDNAME=debug CONFIG="$(CONFIG) --enable-profile --enable-debug" misc/buildarch 

gdb:
	+MAKEFLAGS="$(MAKEFLAGS)" interactive=yes BUILDNAME=debug CONFIG="$(CONFIG) --enable-profile --enable-debug" misc/buildarch gdb 

profile:
	+MAKEFLAGS="$(MAKEFLAGS)" BUILDNAME=profile CONFIG="$(CONFIG) --enable-profile" misc/buildarch  

small:
	+MAKEFLAGS="$(MAKEFLAGS)" BUILDNAME=small CONFIG="$(CONFIG) --enable-small" misc/buildarch 

importgnulib:
	DIR=/tmp/gnulib.$$$$; mkdir $$DIR; (cd $$DIR && cvs -z3 -d:pserver:anonymous@cvs.savannah.gnu.org:/sources/gnulib co gnulib); \
		$$DIR/gnulib/gnulib-tool --libtool --m4-base=libgnu/m4 --doc-base=libgnu/doc --source-base=libgnu --import getdate gettime xalloc-die strftime
	cvs add libgnu/[a-z]* libgnu/m4/[a-z]*

pushrelease: #dist rpm
	@set -x; cd doc && rsync -a -e ssh -Ltv ../build/*/doc/*.pdf *.png *.html api ../ChangeLog mfisk,smacq@web.sf.net:htdocs/
	@set -x; scp build/*/smacq-*.tar.gz build/*/smacq-*.deb build/*/smacq-*.rpm mfisk,smacq@web.sf.net:htdocs/downloads/
	@set -x; scp build/*/smacq-*.rpm packrat1.ds:/var/redhat/netnanny/RPMS/

#dist:
	(set -ex; misc/buildarch dist; d=/tmp/$$$$; mkdir $$d; tar -C $$d -xzf build/*/smacq-*.tar.gz; cd $$d/smacq-*; ./configure; make clean dist); cp -ip /tmp/$$$$/smacq-*/smacq-*.tar.gz . 1>&2

mydistcheck:
	(set -ex; misc/buildarch dist; d=/tmp/$$$$; mkdir $$d; tar -C $$d -xzf build/*/smacq-*.tar.gz; cd $$d/smacq-*; ./configure; make check); echo "IT'S A KEEPER:" build/*/smacq-*.tar.gz 1>&2

bootstrap:
	rm -Rf autom4te.cache config/[a-z]*
	mkdir -p config
	export PATH=/sw/bin:$$PATH; \
	(glibtoolize --copy --force || libtoolize --copy --force); \
	aclocal$(AUTOMAKE_VERSION) -I libgnu/m4; \
	autoheader$(AUTOCONF_VERSION); \
	automake$(AUTOMAKE_VERSION) -a; \
	autoconf$(AUTOCONF_VERSION)

cvsinterforce: cvsinter
		# Have to force timestamp update even if contents unchanged
	cvs commit -m 'update timestamp' -f stamp-h.in aclocal.m4 *.in `find * -maxdepth 2 -name \*.in` configure

cvsinter:
		# All the intermediate files that may change
	cvs commit -m 'intermediate file' *.in `find * -maxdepth 2 -name \*.in` aclocal.m4 configure stamp-h.in config doc/smacq.1* doc/smacqq.1* #libsmacq/parser.h libsmacq/*parser.[yc]pp libsmacq/scanner.cpp

%:
	+MAKEFLAGS="$(MAKEFLAGS)" misc/buildarch $@

smacq.iso: dist
	TOPSRCDIR=`pwd` misc/mkiso
