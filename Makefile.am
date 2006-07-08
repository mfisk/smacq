SUBDIRS = include libgnu types modules libsmacq bin doc test
EXTRA_DIST = misc test smacq.spec smacq.list debian
ACLOCAL_AMFLAGS = -I $(top_srcdir)/libgnu/m4

test: check

gdb:
	interactive=yes $(MAKE) check

quiet:
	$(MAKE) all >/dev/null

native inst slackware setld depot swinstall tardist osx aix bsd deb: smacq.list
	srcdir=$(top_srcdir) epm -v --output-dir . -f $@ smacq $<

rpm: smacq.list
	srcdir=$(top_srcdir) epm -v --output-dir . -f $@ smacq $<
	rpmbuild -bb smacq.spec 

fink: smacq.list
	fink=yes srcdir=$(top_srcdir) epm -f deb -a darwin-`uname -p` --output-dir . smacq $<

tarball: all
	$(MAKE) DESTDIR=install/ install
	
deb.real: 
	cp -pr $(top_srcdir)/debian .
	fakeroot make -f debian/rules top_srcdir=$(top_srcdir)/ binary
