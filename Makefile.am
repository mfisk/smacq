SUBDIRS = include libgnu types modules libsmacq bin pysmacq doc test 
EXTRA_DIST = misc test smacq.spec smacq.list.in debian
ACLOCAL_AMFLAGS = -I $(top_srcdir)/libgnu/m4

test: check

gdb:
	interactive=yes $(MAKE) check

quiet:
	$(MAKE) all >/dev/null

native inst slackware setld depot swinstall tardist osx aix bsd: smacq.list
	srcdir=$(top_srcdir) epm -v --output-dir . -f $@ smacq $<

deb: smacq.list
	. /etc/lsb-release; srcdir=$(top_srcdir) epm -v --output-dir . -a `dpkg --print-architecture` -f $@ smacq -m `echo $$DISTRIB_DESCRIPTION | sed 's/[^a-zA-Z0-9]/_/g'`-`dpkg --print-architecture` $<

rpm: smacq.list
	srcdir=$(top_srcdir) epm -v --output-dir . -f $@ smacq $<
#rpmbuild -bb smacq.spec 

fink: smacq.list
	fink=yes srcdir=$(top_srcdir) epm -f deb -a darwin-`uname -p` --output-dir . smacq $<

tarball: all
	$(MAKE) DESTDIR=`pwd`/install/ install
	tar -C install -czvf smacq.tar.gz usr

deb.real: 
	cp -pr $(top_srcdir)/debian .
	fakeroot make -f debian/rules top_srcdir=$(top_srcdir)/ binary

python:
	PYTHONPATH=pysmacq/.libs:$(top_srcdir)/pysmacq $(PYTHON)

