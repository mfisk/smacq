default: 
	misc/buildarch

cvsinter:
	#cvs commit -m 'intermediate file' -f stamp-h.in # Have to force this one since contents never change
	cvs commit -m 'intermediate file' *.in `find * -name \*.in` libsmacq/*parser.[ych] libsmacq/scanner.c configure
	
%:
	misc/buildarch $@
