include config.mk

all: 
	@TEST=$(SRCDIR); export TEST; \
	pass=0; fail=0; \
	for f in $(SRCDIR)/*.test; do \
		out=`basename $$f`.out; \
		(set -e; cd ..; $$f) > $$out 2>&1; \
		diff -U3 $$f.out $$out; \
		if [ "$$?" = "0" ]; then \
			pass=`expr $$pass + 1`; \
		else \
			fail=`expr $$fail + 1`; \
		fi; \
	done; echo $$fail tests failed, $$pass tests passed; \
	exit $$fail
