default: 
	misc/buildarch

bootstrap:
	libtoolize --automake
	aclocal
	autoconf
	automake

cvsinter:
		# Have to force timestamp update even if contents unchanged
	cvs commit -m 'update timestamp' -f stamp-h.in aclocal.m4 *.in `find * -name \*.in`  
		# All the intermediate files that may change
	cvs commit -m 'intermediate file' *.in `find * -name \*.in` libsmacq/*parser.[ych] libsmacq/scanner.c configure stamp-h.in aclocal.m4 ltmain.sh
	
%:
	misc/buildarch $@
