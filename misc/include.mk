VPATH=$(SRCDIR)

dirs: $(patsubst %, %.RECURSE, $(DIRS))

%.RECURSE: 
	@BINDIR=$(BINDIR)/$*; \
	SRCDIR=$(SRCDIR)/$*; \
	mkdir -p $$BINDIR; \
	LIBTOOL="$(LIBTOOL)" \
	COPTS="$(COPTS)" \
	VPATH=$$SRCDIR make -C $$BINDIR -f $$SRCDIR/Makefile; 

