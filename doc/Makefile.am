DOCS=smacq.1 smacqq.1 dts-modules.3 

DOCDSTS=$(patsubst %,$(srcdir)/%,$(DOCS))
DOCCATS=$(patsubst %,$(srcdir)/%.catman,$(DOCS))
DOCPDFS=smacq.1.pdf smacqq.1.pdf
DOCHTML=index.html gotsmacq.png icon.png

DOXYGEN=doxygen
DOCBOOK2MAN=docbook-to-man

dist_man_MANS=$(DOCDSTS) $(DOCPDFS) $(DOCCATS) api/refman.pdf api/man3 api/html $(DOCHTML)

api: .ALWAYS
	(cd $(srcdir); doxygen)

.ALWAYS:

publish:
	(cd $(srcdir); rsync -a -e ssh -Ltv *.pdf *.png *.html html $(top_srcdir)/build/iso/smacq*.tar.gz ../smacq.iso smacq.sourceforge.net:smacqweb/)

api/refman.pdf: api api/latex/refman.tex
	make -C $(srcdir)/api/latex && cp $(srcdir)/api/latex/refman.pdf $(srcdir)/api/
api/man3: api
api/html: api

%.3.ps: %.3
	groff -man $< > $@

%.pdf: %.ps
	ps2pdf $< $@

%.3.catman: %.3
	nroff -man $< > $@

%.1.ps: %.1
	groff -man $< > $@

%.1.catman: %.1
	nroff -man $< > $@

%.3: %.sgml
	$(DOCBOOK2MAN) $< > $@

%.1: %.sgml
	$(DOCBOOK2MAN) $< > $@

