apidocdir=$(docdir)/api
apihtmldocdir=$(apidocdir)/html

dist_man1_MANS=$(srcdir)/smacq.1 $(srcdir)/smacqq.1 $(srcdir)/smacqq.1.catman $(srcdir)/smacq.1.catman
dist_man3_MANS=$(srcdir)/dts-modules.3 $(srcdir)/api/man3/[^_]*.3
dist_doc_DATA=smacq.1.pdf smacqq.1.pdf index.html gotsmacq.png icon.png 
apidoc_DATA=api/refman.pdf
apihtmldoc_DATA=$(wildcard $(srcdir)/api/html/*) #XXX. Not very portable

EXTRA_DIST=api

DOXYGEN=doxygen
DOCBOOK2MAN=docbook-to-man

api: 
	(mkdir -p $(srcdir)/api && cd $(srcdir) && $(DOXYGEN))

api/refman.pdf: api api/latex/refman.tex
	(make -C $(srcdir)/api/latex && cp $(srcdir)/api/latex/refman.pdf $(srcdir)/api/) || \
	(make -C $(srcdir)/api/latex && cp $(srcdir)/api/latex/refman.pdf $(srcdir)/api/) || \
	(cat $(srcdir)/api/latex/refman.log 1>&2 && false)

$(srcdir)/api/man3/[^_]*.3: api
#api/man3: api
api/html: api

%.3.ps: %.3
	groff -man $< > $@ || rm -f $@

%.pdf: %.ps
	ps2pdf $< $@ || rm -f $@

%.3.catman: %.3
	nroff -man $< > $@ || rm -f $@

%.1.ps: %.1
	groff -man $< > $@ || rm -f $@

%.1.catman: %.1
	nroff -man $< > $@ || rm -f $@

%.3: %.sgml
	$(DOCBOOK2MAN) $< > $@ || rm -f $@

%.1: %.sgml
	$(DOCBOOK2MAN) $< > $@ || rm -f $@

