include config.mk

LDFLAGS=$(LDOPTS) ../libsmacq/libsmacq.la -ldl
BUILTINS=../modules/libsmacq-modules.la ../types/libdts-modules.la 
PRODUCTS=smacqp smacqq iterative-example smacqq-static smacqq-dynamic

BUILTIN_FLAGS=-dlopen self $(patsubst %,-dlopen %,$(BUILTINS))

all: $(PRODUCTS)

%: %.lo  ../include/smacq.h $(LIBSMACQ) $(BUILTINS)
	$(LIBTOOL) $(CXX) -o $@ -static $< $(BUILTIN_FLAGS) $(LDFLAGS)

%-static: %.lo ../include/smacq.h $(LIBSMACQ) $(BUILTINS)
	$(LIBTOOL) $(CXX) -o $@ -all-static $< $(BUILTIN_FLAGS) $(LDFLAGS) 

%-dynamic: %.lo ../include/smacq.h $(LIBSMACQ)  
	$(LIBTOOL) $(CC) -o $@ -dynamic $< $(LDFLAGS) 

%.lo: %.c ../include/smacq.h 
	$(LIBTOOL) $(CC) -c $(CFLAGS) $< 

%: %.c 	# Otherwise Make will try to do it this way instead of via .lo
%: %.o

reallyclean: clean

clean: 
	rm -rf $(PRODUCTS) .libs *.o *.la *.a *.lo


