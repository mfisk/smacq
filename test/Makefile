all: 
	@set -e; for f in *.test; do \
		(cd ..; test/$$f) > /tmp/$$f.out 2>&1; \
		diff -U3 /tmp/$$f.out $$f.out; \
	done; echo All tests passed
