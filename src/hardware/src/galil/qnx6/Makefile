#$Id: Makefile,v 1.6 2003-07-01 10:30:20 beltran Exp $

include $(YARP_ROOT)/conf/Makefile.template

#SYSDIR = qnx6

OBJS =
CC = g++
CFLAGS = --static -g -I $(YARP_ROOT)/include -I ../../common -I ./dd_orig/include 

SUBDIRS =

all: YARPGalilDeviceDriver.lib

YARPGalilDeviceDriver.o: YARPGalilDeviceDriver.h
	@echo
	@echo ---------------Compiling YARPGalilDeviceDriver.lib --------------
	@echo
	$(CC) $(CFLAGS) -c YARPGalilDeviceDriver.cpp -o YARPGalilDeviceDriver.o

YARPGalilDeviceDriver.lib : YARPGalilDeviceDriver.o
	$(AR) $(ARFLAGS) YARPGalilDeviceDriver.lib YARPGalilDeviceDriver.o ./dd_orig/lib/dmcqnx.lib
	
clean:
	@echo
	@echo ---------------Cleaning YARPGalilDeviceDriver.lib --------------
	@echo
	rm *.o
	rm *.lib
	
install:
	@echo
	@echo ---------------Installing YARPGalilDeviceDriver and dd_orig --------------
	@echo
	cp *.lib $(YARP_ROOT)/src/hardware/lib/$(OS)
	cp *.lib $(YARP_ROOT)/lib/$(OS)
	cp *.h	 $(YARP_ROOT)/src/hardware/include
	cp *.h 	 $(YARP_ROOT)/include
	cp ./dd_orig/lib/*.lib $(YARP_ROOT)/src/hardware/lib/$(OS)
	cp ./dd_orig/lib/*.lib $(YARP_ROOT)/lib/$(OS)





