include config.mk

DOCS=smacq.1 smacqq.1 smacqp.1 smacq-modules.3 dts.3 dts-modules.3 smacq-embed.3
DOCDSTS=$(patsubst %,$(SRCDIR)/%,$(DOCS))
DOCTXTS=$(patsubst %,$(SRCDIR)/%.txt,$(DOCS))
DOCPDFS=$(patsubst %,$(SRCDIR)/%.pdf,$(DOCS))

all: $(DOCDSTS) $(PRODUCTS)

reallyclean: clean
	rm -f $(DOCDSTS) 

clean:
	rm -f $(DOCTXTS) $(DOCPDFS)

publish:
	rsync -e ssh -av *.pdf *.png *.html smacq.sourceforge.net:smacqweb/

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

