all: 
	set -e; for f in *.test; do \
		(cd ..; $(SRCDIR)/$$f) > $$f.out 2>&1; \
		diff -U3 $(SRCDIR)/$$f.out $$f.out; \
	done; echo All tests passed
