#VPATH=$(SRCDIR)

All: all

dirs: $(patsubst %, %.RECURSE, $(DIRS))

%.RECURSE.OLD: 
	@BINDIR=$(BINDIR)/$*; \
	SRCDIR=$(SRCDIR)/$*; \
	mkdir -p $$BINDIR; \
	VPATH=$$SRCDIR make -C $$BINDIR -f $$SRCDIR/Makefile \
	LIBTOOL="$(LIBTOOL)" \
	COPTS="$(COPTS)" \

#$*/Makefile: $(SRCDIR)/$*/Makefile
#	cp -f $< $@

%.RECURSE: 
	@mkdir -p $(BINDIR)/$*; \
	echo "include $(TOPBINDIR)/config.mk" 	> $(BINDIR)/$*/config.mk; \
	echo "BINDIR=$(BINDIR)/$*" 		>> $(BINDIR)/$*/config.mk; \
	echo "SRCDIR=$(SRCDIR)/$*" 		>> $(BINDIR)/$*/config.mk; \
	echo "VPATH=$(SRCDIR)/$*" 		>> $(BINDIR)/$*/config.mk; \
	ln -sf $(SRCDIR)/$*/Makefile $(BINDIR)/$*/; \
	make -C $(BINDIR)/$* #-f $(SRCDIR)/$*/Makefile 

$(BINDIR)/Makefile: $(SRCDIR)/Makefile
	cp $< $@

include $(TOPSRCDIR)/config.mk
