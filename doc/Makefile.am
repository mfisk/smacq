DOCS=smacq.1 smacqq.1 smacqp.1 smacq-modules.3 dts-modules.3 smacq-embed.3 strucio.3 DTS.3 DtsObject.3 FieldVec.3

DOCDSTS=$(patsubst %,$(srcdir)/%,$(DOCS))
DOCTXTS=$(patsubst %,$(srcdir)/%.txt,$(DOCS))
DOCPDFS=$(patsubst %,$(srcdir)/%.pdf,$(DOCS))

DOXYGEN=doxygen
DOCBOOK2MAN=docbook-to-man

dist_man_MANS=$(DOCDSTS) $(DOCPDFS) $(DOCTXTS)

publish:
	(cd $(srcdir); rsync -a -e ssh -Ltv *.pdf *.png *.html html $(top_srcdir)/build/iso/smacq*.tar.gz ../smacq.iso smacq.sourceforge.net:smacqweb/)

%.3.ps: %.3
	groff -man $< > $@

%.pdf: %.ps
	ps2pdf $< $@

%.3.txt: %.3
	nroff -man $< > $@

%.1.ps: %.1
	groff -man $< > $@

%.1.txt: %.1
	nroff -man $< > $@

%.3: %.sgml
	$(DOCBOOK2MAN) $< > $@

%.1: %.sgml
	$(DOCBOOK2MAN) $< > $@

html/classDtsObject__.html:
	(cd ..; $(DOXYGEN) doc/Doxyfile)

%.3: man/%.3
	ln -f $< $@
