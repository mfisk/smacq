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

all: libsql.o

gram.c:	gram.y scan.l
	-bison -t -y -d $< -o $@

gram.o:	gram.c
	$(CC) -o $@ -c $(CFLAGS) $<

scan.c: scan.l gram.c 
#	flex -d -i -o$@ $<
	-flex -i -o$@ $<

scan.o:	scan.c
	$(CC) -o $@ -c $(CFLAGS) $<

libsql.o: $(OBJS)
	$(LD) -r -o $@ $+

%.o: %.c
	$(CC) -o $@ -c $(CFLAGS) $<

clean: 
	rm -f *.o *.a scan.c gram.c gram.h nohup.out

