CFLAGS=-g -O3 -Winline -Wall -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I../include $(COPTS)
CLDFLAGS=-Xlinker -E $(CFLAGS)

BUILTIN_MODULES=../modules/smacq-modules.a ../types/dts-modules.a ../libsmacq/libsmacq.a

STATIC_FLAGS=-L../libsmacq -lgmodule-2.0 -lglib-2.0 -lsmacq 
STATIC_LDFLAGS=-E $(STATIC_FLAGS)
DYNAMIC_LDFLAGS=-lpthread -lz -lm -ldl -lutil

PRODUCTS=smacqp-pass1.o smacqp smacqp-dynamic smacqp-static-pass1.o smacqp-static smacqq-pass1.o smacqq iterative-example-pass1.o iterative-example 

all: $(PRODUCTS)

#
# Yes, this really should be STATIC_DEPENDS, in case there is no dynamic library
# and we link with the static one automatically:
#
smacqp-dynamic: smacqp.o ../include/smacq.h 
	$(CC) -o $@ $< $(CLDFLAGS) $(STATIC_FLAGS) $(DYNAMIC_LDFLAGS) $(LDWRAP)

%-pass1.o: %-preloaded.o  
	ld -o $@ -r -static $< $(STATIC_LDFLAGS) $(LDWRAP)

%: %-pass1.o
	$(CC) $(CLDFLAGS) -o $@ $< $(DYNAMIC_LDFLAGS) 

# We could just do "gcc -static" except that then we can't even find
# our builtin modules.  So instead we link everything together
# and then build a dynamically lined executable that depends on virtually
# nothing
smacqp-static: smacqp-static-pass1.o 
	$(CC) $(CLDFLAGS) -o $@ $< 

smacqp-static-pass1.o: smacqp-preloaded.o 
	$(LD) -o $@ -r -static $< $(STATIC_LDFLAGS) $(DYNAMIC_LDFLAGS) $(LDWRAP)

%-preloaded.o: %.o $(BUILTIN_MODULES)
	$(LD) -o $@ -E -r -static --whole-archive $+

smacqp.o: smacqp.c ../include/smacq.h
	$(CC) -c $(CFLAGS) $< 

clean: 
	rm -f $(PRODUCTS) *.o *.a

