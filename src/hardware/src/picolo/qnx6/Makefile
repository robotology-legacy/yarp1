#$Id: Makefile,v 1.11 2003-07-02 10:55:50 beltran Exp $

include $(YARP_ROOT)/conf/Makefile.template

#SYSDIR = qnx6

OBJS =
CC = g++
CFLAGS = -g -I $(YARP_ROOT)/include -I ../../common -I ./dd_orig
#LIBS = $(YARP_ROOT)/lib/$(OS)/bttvx.lib

SUBDIRS =

all: subdirs YARPPicoloDeviceDriver.lib

YARPPicoloDeviceDriver.o: YARPPicoloDeviceDriver.h
	@echo
	@echo ---------------Compiling YARPPicoloDeviceDriver.lib --------------
	@echo
	$(CC) $(CFLAGS) -c -o YARPPicoloDeviceDriver.o YARPPicoloDeviceDriver.cpp

YARPPicoloDeviceDriver.lib : YARPPicoloDeviceDriver.o
	$(AR) $(ARFLAGS) YARPPicoloDeviceDriver.lib YARPPicoloDeviceDriver.o $(LIBS)
	
clean:
	@echo
	@echo ---------------Cleaning YARPPicoloDeviceDriver.lib --------------
	@echo
	rm -f *.o
	rm -f *.lib
	+cd ./dd_orig; make clean
	
install:
	@echo
	@echo ---------------Instaling YARPPicoloDEviceDriver.ib --------------
	@echo
	$(CP) *.lib $(YARP_ROOT)/lib/$(OS)
	$(CP) *.h 	 $(YARP_ROOT)/include
	#cp ./dd_orig/*.lib $(YARP_ROOT)/src/hardware/lib/$(OS)
	#cp ./dd_orig/*.lib $(YARP_ROOT)/lib/$(OS)
	+cd ./dd_orig; make install
	
subdirs:
	+cd ./dd_orig; make 




