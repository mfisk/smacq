CFLAGS= -Winline -Wall -I$(USR)/include/glib-2.0 -I$(USR)/lib/glib-2.0/include -I../include $(COPTS)

LIBTOOL=libtool
LIBSMACQ=../libsmacq/libsmacq.la
BUILTINS=../modules/libsmacq-modules.la ../types/libdts-modules.la 
DYNAMIC_LDFLAGS=../libsmacq/libsmacq.la
LDFLAGS=$(CFLAGS) $(LDOPTS) -export-dynamic $(DYNAMIC_LDFLAGS)

PRODUCTS=smacqp smacqq iterative-example	smacqp-dynamic

all: $(PRODUCTS)

%: %.c ../include/smacq.h $(LIBSMACQ) $(BUILTINS)
	$(LIBTOOL) $(CC) -o $@ -static $(LDFLAGS) $(BUILTINS) $< 

%-static: %.c ../include/smacq.h $(LIBSMACQ) $(BUILTINS)
	$(LIBTOOL) $(CC) -o $@ -all-static $(LDFLAGS) $(BUILTINS) $<  

%-dynamic: %.lo ../include/smacq.h $(LIBSMACQ)  
	$(LIBTOOL) $(CC) -o $@ -dynamic $(LDFLAGS) $<

%.lo: %.c ../include/smacq.h 
	$(LIBTOOL) $(CC) -c $(CFLAGS) $< 

reallyclean: clean

clean: 
	rm -rf $(PRODUCTS) .libs *.o *.la *.a *.lo


