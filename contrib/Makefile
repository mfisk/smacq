include config.mk

ETHEREAL=ethereal-0.9.12
ARCAT=$(TOPSRCDIR)/misc/arcat

all: $(WITH)

ethereal: libpacket-ethereal.a

reallyclean: clean
clean:
	rm -Rf .libs libpcap*

snort-%: snort-%.tar.gz
	tar xzf $< 
	cd $@; ./configure --enable-debug && $(MAKE)

snort-%.tar.gz:
	wget -N http://www.snort.org/dl/$@.tar.gz

libpcap.la: libpcap-current.tar.gz 
	mkdir -p libpcap
	tar -C libpcap -xzf $<
	cd libpcap/libpcap-*; ./configure && $(MAKE)
	cp libpcap/libpcap-*/$@ .

libpcap-current.tar.gz: 
	wget -N http://public.lanl.gov/cpw/$@

$(SRCDIR)/ethereal-%.tar.gz:
	cd $(SRCDIR); wget -N http://www.ethereal.com/distribution/`basename $@`

libethereal.a: $(ETHEREAL)
	$(ARCAT) $@ $</*.o $</reassemble.o $</tap.o $</register.o `find $< -name \*.a -print`

packet-ethereal.o: packet-ethereal.c $(ETHEREAL)
	$(CC) -c $< $(CFLAGS) -I$(ETHEREAL)
	
packet-ethereal-bundle-strong.o: packet-ethereal.o libethereal.a
	$(LD) -r -o $@ $< -L. -lethereal -L$(USR)//lib -lglib -lpcap

lib%.a: %-bundle-weak.o
	$(AR) r $@ $<

%-weak.o: %-strong.o
	objcopy --weaken $< $@

ethereal-%: $(SRCDIR)/ethereal-%.tar.gz
	tar xzf $<
	cd $@; ./configure --disable-editcap --disable-mergecap --disable-text2pcap --disable-ethereal --disable-idl2eth && $(MAKE)

