reloc:  $(BINDIR)/../bin/smacqq $(SRCDIR)/../misc/reloc
	@set -e; LIBS=`ldd $(BINDIR)/../bin/smacqq | cut -d'>' -f2 | cut -d'(' -f1`; \
	echo rm -Rf *; \
	mkdir -p .bin .lib; \
	cp $$LIBS .lib/
	cp $+ .bin/; \
	rm -f smacqq; \
	ln -s .bin/reloc smacqq; 

$(BINDIR)/../bin/smacqq:
	echo "No smacqq!"
	ls $@

clean:

iso: reloc
	cp $(SRCDIR)/../misc/smacq.ico $(SRCDIR)/../misc/autorun.inf $@/; \
