#$Id: Makefile,v 1.3 2003-05-17 14:22:55 beltran Exp $

include $(YARP_ROOT)/conf/Makefile.template

SYSDIR = qnx6

OBJS =
CC = g++
CFLAGS = --static -g -I $(YARP_ROOT)/include -I ../../common -I $(YARP_ROOT)/src/libraries/os_services/include \
-I $(YARP_ROOT)/src/libraries/os_services/sys_include -I ./dd_orig/include 

SUBDIRS =

all: YARPGalilDeviceDriver.lib

YARPGalilDeviceDriver.o: YARPGalilDeviceDriver.h
	$(CC) $(CFLAGS) -c YARPGalilDeviceDriver.cpp -o YARPGalilDeviceDriver.o

YARPGalilDeviceDriver.lib : YARPGalilDeviceDriver.o
	$(AR) $(ARFLAGS) YARPGalilDeviceDriver.lib YARPGalilDeviceDriver.o ./dd_orig/lib/dmcqnx.lib
	
clean:
	rm *.o
	
install:
	cp *.lib $(YARP_ROOT)/src/hardware/lib
	cp *.h	 $(YARP_ROOT)/src/hardware/include
	cp ./dd_orig/lib/*.lib $(YARP_ROOT)/src/hardware/lib





