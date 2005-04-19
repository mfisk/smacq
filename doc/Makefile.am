DOCS=smacq.1 smacqq.1 dts-modules.3 
DOXYGEN=doxygen
DOCBOOK2MAN=docbook-to-man

DOCDSTS=$(patsubst %,$(srcdir)/%,$(DOCS))
#DOCCATS=$(patsubst %,$(srcdir)/%.catman,$(DOCS))

EXTRA_DIST=api/refman.pdf api/html smacq1.pdf smacqq.1.pdf index.html gotsmacq.png icon.png
dist_man_MANS=api/man3 $(DOCDSTS) #$(DOCCATS) 

api: .ALWAYS
	(cd $(srcdir); $(DOXYGEN)

.ALWAYS:

publish:
	cp *.pdf $(srcdir)/
	(cd $(srcdir); rsync -a -e ssh -Ltv *.pdf *.png *.html api ../ChangeLog smacq.sourceforge.net:smacqweb/)

api/refman.pdf: api api/latex/refman.tex
	(make -C $(srcdir)/api/latex && cp $(srcdir)/api/latex/refman.pdf $(srcdir)/api/) || \
	(make -C $(srcdir)/api/latex && cp $(srcdir)/api/latex/refman.pdf $(srcdir)/api/) || \
	(cat $(srcdir)/api/latex/refman.log 1>&2 && false)
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

