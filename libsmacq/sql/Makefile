PGHOME = .

INCLUDES = -I. \
	-I$(PGHOME)/include \
	-I$(PGHOME)/include/nodes \
	-I$(PGHOME)/include/tcop \
	-I$(PGHOME)/include/storage \
	-I$(PGHOME)/include/utils \
	-I$(PGHOME)/include/lib \
	-I$(PGHOME)/include/parser \
	-I$(PGHOME)/include/rewrite

OBJS = scan.o gram.o transform.o\
  keywords.o scansup.o list.o nodes.o equalfuncs.o datum.o copyfuncs.o

CFLAGS = -ggdb -Wall -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include $(INCLUDES) -DFRONTEND

LIBS = -lgmodule-2.0 -lglib-2.0 -lz -lcrypt -lresolv -lnsl -ldl -lm

all: gram.c scan.c libsql.o

gram.c:	gram.y scan.l
	bison -t -y -d  $< && mv y.tab.c $*.c
	if test -f y.tab.h; then \
	if cmp -s y.tab.h $*.h; then rm -f y.tab.h; else mv y.tab.h $*.h; fi; \
	else :; fi

gram.o:	gram.c
#	gcc -o $@ -c -DYYDEBUG $(CFLAGS) $<
	gcc -o $@ -c $(CFLAGS) $<

scan.c: scan.l gram.y
#	flex -d -i -o$@ $<
	flex -i -o$@ $<

scan.o:	scan.c
#	gcc -o $@ -c -DYYDEBUG $(CFLAGS) $<
	gcc -o $@ -c $(CFLAGS) $<

libsql.o: $(OBJS)
	$(LD) -r -o $@ $+

%.o: %.c
	gcc -o $@ -c $(CFLAGS) $<

clean: 
	rm -f *.o *.a scan.c gram.c gram.h nohup.out

