all: 
	@set -e; TEST=$(SRCDIR); export TEST; \
	for f in $(SRCDIR)/*.test; do \
		out=`basename $$f`.out; \
		(cd ..; $$f) > $$out 2>&1; \
		diff -U3 $$f.out $$out; \
	done; echo All tests passed
