include $(YARP_ROOT)/conf/Makefile.template


CFLAGS = -w9 $(CFAST) -g -M -I../include -I$(YARP_ROOT)/src/hardware/include
LDFLAGS = -L../lib -L$(YARP_ROOT)/src/hardware/lib/$(OS) -lmeid
##LDFLAGS = -L/home/cog/lib -lmeid

OFILES = init.o pos.o
HFILES = ../include/meid.h

.SUFFIXES:
.SUFFIXES:	.o .c .a


all:  pos posdup


pos:  pos.o
	cc -o pos $(LDFLAGS) pos.o

posdup:  posdup.o
	cc -o posdup $(LDFLAGS) posdup.o



$(OFILES):  $(HFILES)



clean:
	-rm *.o *.err *.map pos

squeaky: clean
	-rm *~

.PHONY:  all clean squeaky
