# Makefile for bbtvx and blit (visualization)

#CC=gcc
#CFLAGS= -ggdb3
LDFLAGS=
SRC=bttvx.c
SRC2=blit.c
OBJS=bttvx.o bttvx_dev.o
OBJS2=blit.o
DIST=bttvx_0.2.2
OS = qnx6

all: $(OBJS) bttvx blit bttvx_dev

bttvx: $(OBJS)
	@echo
	@echo ---------------Creating bttvx.lib --------------
	@echo
	#$(CC) $(LDFLAGS) $(OBJS) -o $@
	$(AR) $(ARFLAGS) $@.lib $@.o

blit:  $(OBJS2)
	$(CC) $(LDFLAGS) $(OBJS2) bttvx.lib -o $@ -lph
	
bttvx_dev:
	@echo
	@echo --------------Compiling bttvx_dev ---------------
	@echo
	$(CC) $(LDFLAGS) $@.o bttvx.lib  -o $@  
	   
install:
	@echo
	@echo ---------------Instaling bttvx --------------
	@echo
	cp *.h $(YARP_ROOT)/include 
	cp *.lib $(YARP_ROOT)/lib/$(OS)

clean:
	@echo
	@echo ---------------Cleaning bttvx --------------
	@echo
	rm -f bttvx
	rm -f bttvx_dev
	rm -f *.o
	rm -f blit
