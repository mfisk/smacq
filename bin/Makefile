CFLAGS=-g -O3 -Winline -Wall -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I../include $(COPTS)

LIBSMACQ=../libsmacq/libsmacq.la
BUILTINS=../modules/libsmacq-modules.lo ../types/libdts-modules.lo 
DYNAMIC_LDFLAGS=$(LIBSMACQ) -lpthread -lz -lm -ldl -lpcap ../libsmacq/libsmacq.la -lgmodule-2.0 -lglib-2.0 #-lutil
LDFLAGS=$(CFLAGS) $(LDOPTS) -export-dynamic $(DYNAMIC_LDFLAGS)

PRODUCTS=smacqp smacqq iterative-example	smacqp-dynamic

all: $(PRODUCTS)

%: %.c ../include/smacq.h $(LIBSMACQ) $(BUILTINS)
	$(LIBTOOL) $(CC) -o $@ -static $(CFLAGS) $(BUILTINS) $(LDFLAGS) $< 

%-static: %.c ../include/smacq.h $(LIBSMACQ) $(BUILTINS)
	$(LIBTOOL) $(CC) -o $@ -all-static $(CFLAGS) $(BUILTINS) $(LDFLAGS) $<  

%-dynamic: %.lo ../include/smacq.h $(LIBSMACQ)  
	$(LIBTOOL) $(CC) -o $@ -dynamic $(LDFLAGS) $<

%.lo: %.c ../include/smacq.h 
	$(LIBTOOL) $(CC) -c $(CFLAGS) $< 

reallyclean: clean

clean: 
	rm -rf $(PRODUCTS) .libs *.o *.la *.a *.lo


