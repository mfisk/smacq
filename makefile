default: 
	misc/buildarch

bootstrap:
	libtoolize --automake
	aclocal-1.7
	autoconf
	automake-1.7

cvsinterforce: cvsinter
		# Have to force timestamp update even if contents unchanged
	cvs commit -m 'update timestamp' -f stamp-h.in aclocal.m4 *.in `find * -name \*.in`  

cvsinter:
		# All the intermediate files that may change
	cvs commit -m 'intermediate file' *.in `find * -name \*.in` libsmacq/*parser.[ych] libsmacq/scanner.c configure stamp-h.in aclocal.m4 ltmain.sh ylwrap missing depcomp install-sh mkinstalldirs config.h.in config.sub config.guess 

%:
	misc/buildarch $@

smacq.iso: dist
	TOPSRCDIR=`pwd` misc/mkiso
