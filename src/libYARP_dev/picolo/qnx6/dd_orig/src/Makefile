# Makefile for bbtvx and blit (visualization)

CC=qcc
#CFLAGS= -g
#LDFLAGS= -g
CFLAGS= 
LDFLAGS= 
SRC=bttvx.c
SRC2=blit.c
OBJS=bttvx.o bttvx_dev.o
OBJS2=blit.o
DIST=bttvx_0.2.2
OS = qnx6

all: $(OBJS) bttvx blit bttvx_dev bttvx_regs

bttvx: $(OBJS)
	@echo
	@echo ---------------Creating bttvx.lib --------------
	@echo
	#$(CC) $(LDFLAGS) $(OBJS) -o $@
	$(AR) $(ARFLAGS) $@.a $@.o

blit:  $(OBJS2)
	$(CC) $(LDFLAGS) $(OBJS2) bttvx.a -o $@ -lph
	
bttvx_dev:
	@echo
	@echo --------------Compiling bttvx_dev ---------------
	@echo
	$(CC) $(LDFLAGS) $@.o bttvx.a  -o $@  

bttvx_regs:
	@echo
	@echo ---------------Compiling bttvx_reg --------------
	@echo
	$(CC) $(LDFLAGS) $@.c -o $@
	   
install:
	@echo
	@echo ---------------Instaling bttvx --------------
	@echo
	mkdir -p ../lib
	cp *.a ../lib/

clean:
	@echo
	@echo ---------------Cleaning bttvx --------------
	@echo
	rm -f bttvx
	rm -f bttvx_dev
	rm -f bttvx_regs
	rm -f *.o
	rm -f blit
