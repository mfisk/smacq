include config.mk

ETHEREAL=ethereal-0.9.12
ARCAT=$(TOPSRCDIR)/misc/arcat

all: $(WITH)

ethereal: libpacket-ethereal.la

reallyclean: clean
clean:
	rm -Rf *.la *.lo *.o *.a .libs libpcap*

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

libethereal.la: $(ETHEREAL)
	$(LIBTOOL) $(CC) `find $< \( -name plugins -prune \) , \( -name \*.lo -print \)` -o $@

packet-ethereal.lo: packet-ethereal.c $(ETHEREAL)
	$(LIBTOOL) $(CC) $(CFLAGS) -I$(ETHEREAL) -c $<
	
libpacket-ethereal.la: packet-ethereal.lo libethereal.la
	$(LIBTOOL) $(CC) $< -L. libethereal.la -L$(USR)/lib -lglib -lpcap -o $@

ethereal-%: $(SRCDIR)/ethereal-%.tar.gz
	tar xzf $<
	cd $@; ./configure --disable-editcap --disable-mergecap --disable-text2pcap --disable-ethereal --disable-idl2eth && $(MAKE) OBJEXT=lo


