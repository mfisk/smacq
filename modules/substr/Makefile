include config.mk

all: libsubstr.la

libsubstr.la: substr.lo trie.lo 
	$(LIBTOOL) $(CCLD) -o $@ $+ 

%.lo: %.c
	$(LIBTOOL) $(CC) -c $(CFLAGS) $<

trie.c: trie.h
substr.c: trie.h substr.h

reallyclean: clean

clean:
	rm -Rf *.lo *.la *.o *.a .libs

