include config.mk

ETHEREAL=ethereal-0.9.11
ARCAT=$(TOPSRCDIR)/misc/arcat

WITH.ethereal: libpacket-ethereal.a

all: #libpacket-ethereal.a #libpcap.la #snort-1.9.0

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
	cd $(SRCDIR); wget -N http://www.ethereal.com/distribution/$@

libethereal.a: $(ETHEREAL)
	$(ARCAT) $@ $</*.o $</reassemble.o $</tap.o $</register.o `find $< -name \*.a -print`

packet-ethereal.o: packet-ethereal.c $(ETHEREAL)
	$(CC) -c $< $(CFLAGS) -I$(ETHEREAL)
	
packet-ethereal-bundle-strong.o: packet-ethereal.o libethereal.a
	$(LD) -r -o $@ $< -L. -lethereal -lglib -lpcap

%.o: %-strong.o
	objcopy --weaken $< $@

lib%.a: %-bundle.o
	$(AR) r $@ $<

ethereal-%: ethereal-%.tar.gz
	tar xzf $<
	cd $@; ./configure --disable-editcap --disable-mergecap --disable-text2pcap --disable-ethereal --disable-idl2eth && $(MAKE)

