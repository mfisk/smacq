include config.mk

all: 
	@TEST=$(SRCDIR); export TEST; \
	pass=0; fail=0; \
	for f in $(SRCDIR)/*.test; do \
		err=`basename $$f`.err; \
		out=`basename $$f`.out; \
		(set -e; cd ..; $$f) > $$out 2>$$err; \
		diff -U3 $$f.out $$out; \
		if [ "$$?" = "0" ]; then \
			pass=`expr $$pass + 1`; \
		else \
			echo "======== Errors from $$f: ========"; \
			cat $$err; \
			echo "======== End of errors from $$f ========"; \
			fail=`expr $$fail + 1`; \
		fi; \
	done; echo $$fail tests failed, $$pass tests passed; \
	exit $$fail
