default: 
	misc/buildarch

cvsinter:
	cvs commit -m 'intermediate file' `find . -name \*.in` libsmacq/sql/parser.[ch] libsmacq/sql/scanner.c libsmacq/filter/*-parser.[ch]
	
	
