DOCS=smacq.1 smacqq.1 smacqp.1 smacq-modules.3 dts.3 dts-modules.3 smacq-embed.3
DOCTXTS=$(patsubst %,%.txt,$(DOCS))
DOCPDFS=$(patsubst %,%.pdf,$(DOCS))

PRODUCTS=$(DOCTXTS) $(DOCPDFS)

all: $(DOCS) $(PRODUCTS)

reallyclean: clean
	rm -f smacq-embed.3 dts-types.3 smacqq.1 smacqp.1

clean:
	rm -f $(PRODUCTS)

publish:
	rsync -e ssh -av *.pdf *.png *.html smacq.sourceforge.net:smacqweb/

%.3.ps: %.3
	groff -man $< > $@

%.pdf: %.ps
	ps2pdf $<

%.3.txt: %.3
	nroff -man $< > $@

%.1.ps: %.1
	groff -man $< > $@

%.1.txt: %.1
	nroff -man $< > $@

%.3: %.sgml
	docbook-to-man $< > $@

%.1: %.sgml
	docbook-to-man $< > $@

