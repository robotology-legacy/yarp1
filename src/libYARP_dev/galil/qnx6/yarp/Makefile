#$Id: Makefile,v 1.2 2004-11-23 17:38:31 beltran Exp $

include $(YARP_ROOT)/conf/Makefile.template

#SYSDIR = qnx6

OBJS =
#CC = g++
CFLAGS = $(CFAST) $(CDEFS) -I $(YARP_ROOT)/include -I ../../../include -I ../dd_orig/include -I $(YARP_ROOT)/include/$(SYSDIR) 

SUBDIRS =

all: YARPGalilDeviceDriver.lib

YARPGalilDeviceDriver.o: YARPGalilDeviceDriver.h
	@echo
	@echo ---------------Compiling YARPGalilDeviceDriver.lib --------------
	@echo
	$(CC) $(CFLAGS) -c YARPGalilDeviceDriver.cpp -o YARPGalilDeviceDriver.o

YARPGalilDeviceDriver.lib : YARPGalilDeviceDriver.o
	$(AR) $(ARFLAGS) YARPGalilDeviceDriver.lib YARPGalilDeviceDriver.o ../dd_orig/lib/dmcqnx.lib
	
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





