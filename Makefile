WMAKE=$(MAKE) >&-
DIRS=libsmacq types modules bin doc

all: dirs

dirs: 
	@for f in $(DIRS); do $(MAKE) -C $$f; done

warn: 
	make all >&-

clean: 
	@for f in $(DIRS); do $(MAKE) -C $$f clean; done

dist:
	tar czplf flow.tgz flow/flow doc/*.pdf doc/*.txt doc/*.1 doc/*.3
	(cd /tmp; cvs -q -d cj:/home/cvs co flow); tar -C /tmp -czlf flow-src.tgz flow
