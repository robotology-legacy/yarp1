#$Id: Makefile,v 1.3 2003-06-19 17:35:37 beltran Exp $

include $(YARP_ROOT)/conf/Makefile.template

#SYSDIR = qnx6

OBJS =
CC = qcc
CFLAGS = -ggdb3 -g -I $(YARP_ROOT)/include -I ../../common 

SUBDIRS =

all: YARPPicoloDeviceDriver.lib

YARPPicoloDeviceDriver.o: YARPPicoloDeviceDriver.h
	$(CC) $(CFLAGS) -c -o YARPPicoloDeviceDriver.o YARPPicoloDeviceDriver.cpp

YARPPicoloDeviceDriver.lib : YARPPicoloDeviceDriver.o
	$(AR) $(ARFLAGS) YARPPicoloDeviceDriver.lib YARPPicoloDeviceDriver.o ./dd_orig/bttvx.lib
	
clean:
	rm *.o
	
install:
	cp *.lib $(YARP_ROOT)/src/hardware/lib/$(OS)
	cp *.lib $(YARP_ROOT)/lib/$(OS)
	cp *.h	 $(YARP_ROOT)/src/hardware/include
	cp *.h 	 $(YARP_ROOT)/include
	cp ./dd_orig/*.lib $(YARP_ROOT)/src/hardware/lib/$(OS)
	cp ./dd_orig/*.lib $(YARP_ROOT)/lib/$(OS)





