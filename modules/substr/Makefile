LIBTOOL=libtool

CFLAGS = -ggdb -Wall -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include $(INCLUDES) -I../../include

all: libsubstr.la

libsubstr.la: substr.lo trie.lo 
	$(LIBTOOL) $(CC) -o $@ $+ 

%.lo: %.c
	$(LIBTOOL) $(CC) -c $(CFLAGS) $<

trie.c: trie.h
substr.c: trie.h substr.h

reallyclean: clean

clean:
	rm -Rf *.lo *.la *.o *.a .libs

