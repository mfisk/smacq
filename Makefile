default: 
	misc/buildarch

bootstrap:
	rm -Rf autom4te.cache
	libtoolize --automake
	aclocal
	autoconf
	automake

cvsinterforce: cvsinter
		# Have to force timestamp update even if contents unchanged
	cvs commit -m 'update timestamp' -f stamp-h.in aclocal.m4 *.in `find * -name \*.in` configure

cvsinter:
		# All the intermediate files that may change
	cvs commit -m 'intermediate file' *.in `find * -name \*.in` configure stamp-h.in aclocal.m4 ltmain.sh ylwrap missing depcomp install-sh mkinstalldirs config.h.in config.sub config.guess doc/smacq.1* doc/smacqq.1* #libsmacq/parser.h libsmacq/*parser.[yc]pp libsmacq/scanner.cpp

%:
	misc/buildarch $@

smacq.iso: dist
	TOPSRCDIR=`pwd` misc/mkiso
