include $(YARP_ROOT)/conf/Makefile.template


#CFLAGS = -w9 -g -M -I/home/cog/include -I../include
CFLAGS = -w9 -g $(CFASTSTRUCT) -M -I../include 
LDFLAGS = -L$(YARP_ROOT)/src/hardware/lib -L../client -lueid

OFILES = init.o pos.o
HFILES = ../include/ueid.h

.SUFFIXES:
.SUFFIXES:	.o .c .a

init:  init.o
	cc -o init $(LDFLAGS) init.o

pos:  pos.o
	cc -o pos $(LDFLAGS) pos.o

$(OFILES):  $(HFILES)



clean:
	\rm *.o *.err *.map ueid

