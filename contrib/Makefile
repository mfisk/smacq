all: snort-1.8.6 libpcap.a

clean:
	rm -Rf .libs libpcap*

snort-1.8.6: snort-1.8.6.tar.gz
	tar xzf $< 
	cd $@; ./configure --enable-debug && $(MAKE)

snort-1.8.6.tar.gz:
	wget -N http://www.snort.org/dl/snort-1.8.6.tar.gz

snort-1.8.7: snort-1.8.7.tar.gz
	tar xzf $< 
	cd $@; ./configure --enable-debug && $(MAKE) 

snort-1.8.7.tar.gz:
	wget -N http://www.snort.org/dl/snort-1.8.7.tar.gz

libpcap.a: libpcap-current.tar.gz 
	mkdir -p libpcap
	tar -C libpcap -xzf $<
	cd libpcap/libpcap-*; ./configure && $(MAKE)
	cp libpcap/libpcap-*/$@ .

libpcap-current.tar.gz: 
	wget -N http://public.lanl.gov/cpw/$@

