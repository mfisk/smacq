reloc:  $(TOPBINDIR)/bin/smacqq $(TOPSRCDIR)/misc/reloc
	@set -e; LIBS=`ldd $(TOPBINDIR)/bin/smacqq | cut -d'>' -f2 | cut -d'(' -f1`; \
	echo rm -Rf *; \
	mkdir -p .bin .lib; \
	cp $$LIBS .lib/
	cp $+ .bin/; \
	rm -f smacqq; \
	ln -s .bin/reloc smacqq; 

$(TOPBINDIR)/bin/smacqq:
	echo "No smacqq!"
	ls $@

clean:

iso: reloc
	cp $(TOPSRCDIR)/misc/smacq.ico $(TOPSRCDIR)/misc/autorun.inf $@/; \
