CFLAGS= -Winline -Wall -I$(SRCDIR) -I$(USR)/include/glib-2.0 -I$(USR)/lib/glib-2.0/include -I$(SRCDIR)/../include $(COPTS)

LIBTOOL=libtool
LIBSMACQ=../libsmacq/libsmacq.la
BUILTINS=../modules/libsmacq-modules.la ../types/libdts-modules.la 

BUILTIN_FLAGS=-dlopen self $(patsubst %,-dlopen %,$(BUILTINS))
#BUILTIN_FLAGS=-export-dynamic $(BUILTINS)
#BUILTIN_FLAGS=$(BUILTINS)
#BUILTIN_FLAGS=-export-dynamic $(patsubst %,-dlpreopen %,$(BUILTINS))
#BUILTIN_FLAGS=-export-dynamic $(patsubst %,-dlopen %,$(BUILTINS))
#BUILTIN_FLAGS=$(patsubst ../modules/lib%.la,-L../modules -l%,$(BUILTINS))
#BUILTIN_FLAGS=-export-dynamic $(patsubst ../types/lib%.la,-L../types -l%,$(BUILTINS))

DYNAMIC_LDFLAGS=$(LIBSMACQ)
LDFLAGS=$(CFLAGS) $(LDOPTS) $(DYNAMIC_LDFLAGS)

PRODUCTS=smacqp smacqq iterative-example smacqq-static smacqq-dynamic

all: $(PRODUCTS)

%: %.c  ../include/smacq.h $(LIBSMACQ) $(BUILTINS)
	$(LIBTOOL) $(CC) -o $@ -static $< $(BUILTIN_FLAGS) $(LDFLAGS)

%-static: %.c ../include/smacq.h $(LIBSMACQ) $(BUILTINS)
	$(LIBTOOL) $(CC) -o $@ -all-static $< $(BUILTIN_FLAGS) $(LDFLAGS) 

%-dynamic: %.lo ../include/smacq.h $(LIBSMACQ)  
	$(LIBTOOL) $(CC) -o $@ -dynamic $< $(LDFLAGS) 

%.lo: %.c ../include/smacq.h 
	$(LIBTOOL) $(CC) -c $(CFLAGS) $< 

%: %.c 	# Otherwise Make will try to do it this way instead of via .lo
%: %.o

reallyclean: clean

clean: 
	rm -rf $(PRODUCTS) .libs *.o *.la *.a *.lo


