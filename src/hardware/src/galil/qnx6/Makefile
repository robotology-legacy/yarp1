#$Id: Makefile,v 1.13 2003-10-30 14:09:58 beltran Exp $

include $(YARP_ROOT)/conf/Makefile.template

#SYSDIR = qnx6

OBJS =
#CC = g++
CFLAGS = -I $(YARP_ROOT)/include -I ../../common -I ../$(OS)/dd_orig/include 

SUBDIRS =

all: YARPGalilDeviceDriver.lib

YARPGalilDeviceDriver.o: YARPGalilDeviceDriver.h
	@echo
	@echo ---------------Compiling YARPGalilDeviceDriver.lib --------------
	@echo
	$(CC) $(CFLAGS) -c YARPGalilDeviceDriver.cpp -o YARPGalilDeviceDriver.o

YARPGalilDeviceDriver.lib : YARPGalilDeviceDriver.o
	$(AR) $(ARFLAGS) YARPGalilDeviceDriver.lib YARPGalilDeviceDriver.o ../../obj/$(OS)/*.o ./dd_orig/lib/dmcqnx.lib
	
clean:
	@echo
	@echo ---------------Cleaning YARPGalilDeviceDriver.lib --------------
	@echo
	rm -f *.o
	rm -f *.lib
	
install:
	@echo
	@echo ---------------Installing YARPGalilDeviceDriver and dd_orig --------------
	@echo
	$(CP) *.lib $(YARP_ROOT)/src/hardware/lib/$(OS)
	$(CP) *.lib $(YARP_ROOT)/lib/$(OS)
	$(CP) *.h 	 $(YARP_ROOT)/include





