%.png: %.dia
	dia --nosplash --export $@ $<

%.ico: %.png
	pngtopnm $< | ppmquant 256 | ppmtowinicon > $@


