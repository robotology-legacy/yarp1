#$Id: Makefile,v 1.8 2003-06-30 08:31:40 beltran Exp $

include $(YARP_ROOT)/conf/Makefile.template

#SYSDIR = qnx6

OBJS =
CC = g++
CFLAGS = -g -I $(YARP_ROOT)/include -I ../../common -I ./dd_orig/include
#LIBS = $(YARP_ROOT)/lib/$(OS)/bttvx.lib

SUBDIRS =

all: YARPPicoloDeviceDriver.lib

YARPPicoloDeviceDriver.o: YARPPicoloDeviceDriver.h
	$(CC) $(CFLAGS) -c -o YARPPicoloDeviceDriver.o YARPPicoloDeviceDriver.cpp

YARPPicoloDeviceDriver.lib : YARPPicoloDeviceDriver.o
	$(AR) $(ARFLAGS) YARPPicoloDeviceDriver.lib YARPPicoloDeviceDriver.o $(LIBS)
	
clean:
	rm *.o
	rm *.lib
	
install:
	cp *.lib $(YARP_ROOT)/src/hardware/lib/$(OS)
	cp *.lib $(YARP_ROOT)/lib/$(OS)
	cp *.h	 $(YARP_ROOT)/src/hardware/include
	cp *.h 	 $(YARP_ROOT)/include
	cp ./dd_orig/*.lib $(YARP_ROOT)/src/hardware/lib/$(OS)
	cp ./dd_orig/*.lib $(YARP_ROOT)/lib/$(OS)





