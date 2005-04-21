SUBDIRS = include libgnu libsmacq types modules bin doc test
EXTRA_DIST = misc test smacq.spec debian
ACLOCAL_AMFLAGS = -I $(top_srcdir)/libgnu/m4

test: check

quiet:
	$(MAKE) all >/dev/null

deb: 
	fakeroot debian/rules binary
