#$Id: Makefile,v 1.5 2003-06-19 17:40:39 beltran Exp $

include $(YARP_ROOT)/conf/Makefile.template

#SYSDIR = qnx6

OBJS =
CC = g++
CFLAGS = --static -g -I $(YARP_ROOT)/include -I ../../common -I ./dd_orig/include 

SUBDIRS =

all: YARPGalilDeviceDriver.lib

YARPGalilDeviceDriver.o: YARPGalilDeviceDriver.h
	$(CC) $(CFLAGS) -c YARPGalilDeviceDriver.cpp -o YARPGalilDeviceDriver.o

YARPGalilDeviceDriver.lib : YARPGalilDeviceDriver.o
	$(AR) $(ARFLAGS) YARPGalilDeviceDriver.lib YARPGalilDeviceDriver.o ./dd_orig/lib/dmcqnx.lib
	
clean:
	rm *.o
	
install:
	cp *.lib $(YARP_ROOT)/src/hardware/lib/$(OS)
	cp *.lib $(YARP_ROOT)/lib/$(OS)
	cp *.h	 $(YARP_ROOT)/src/hardware/include
	cp *.h 	 $(YARP_ROOT)/include
	cp ./dd_orig/lib/*.lib $(YARP_ROOT)/src/hardware/lib/$(OS)
	cp ./dd_orig/lib/*.lib $(YARP_ROOT)/lib/$(OS)





