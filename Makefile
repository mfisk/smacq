default: 
	misc/buildarch

cvsinter:
	cvs commit -m 'intermediate file' -f stamp-h.in # Have to force this one since contents never change
	cvs commit -m 'intermediate file' `find . -name \*.in` libsmacq/sql/parser.[ych] libsmacq/sql/scanner.c libsmacq/filter/*-parser.[ych] configure
	
%:
	misc/buildarch $@
