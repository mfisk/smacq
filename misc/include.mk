VPATH=$(SRCDIR)

dirs: $(patsubst %, %.RECURSE, $(DIRS))

%.RECURSE: 
	@BINDIR=$(BINDIR)/$*; \
	SRCDIR=$(SRCDIR)/$*; \
	mkdir -p $$BINDIR; \
	VPATH=$$SRCDIR make -C $$BINDIR -f $$SRCDIR/Makefile \
	LIBTOOL="$(LIBTOOL)" \
	COPTS="$(COPTS)" \

