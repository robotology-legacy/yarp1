# Makefile for bbtvx and blit (visualization)

CC=qcc
CFLAGS= -ggdb3
LDFLAGS=
SRC=bttvx.c
SRC2=blit.c
OBJS=bttvx.o
OBJS2=blit.o
DIST=bttvx_0.2.2

all: $(OBJS) bttvx blit

bttvx: $(OBJS)
	   $(CC) $(LDFLAGS) $(OBJS) -o $@

blit:  $(OBJS2)
	   $(CC) $(LDFLAGS) $(OBJS2) -o $@ -lph 

clean:
	rm -f bttvx
	rm -f ${DIST}.tgz
	rm -rf *.o
	rm -f blit
