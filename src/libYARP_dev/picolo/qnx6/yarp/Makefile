#$Id: Makefile,v 1.1 2004-07-13 13:21:10 babybot Exp $

include $(YARP_ROOT)/conf/Makefile.template

#SYSDIR = qnx6

OBJS =
#CC = g++
CFLAGS += -I ./dd_orig -I $(YARP_ROOT)/include -I ../../common 
#LIBS = $(YARP_ROOT)/lib/$(OS)/bttvx.lib

SUBDIRS =

all: subdirs YARPPicoloDeviceDriver.lib

YARPPicoloDeviceDriver.o: YARPPicoloDeviceDriver.h
	@echo
	@echo ---------------Compiling YARPPicoloDeviceDriver.lib --------------
	@echo
	$(CC) $(EXTRA_OPTS) $(CFLAGS) -c -o YARPPicoloDeviceDriver.o YARPPicoloDeviceDriver.cpp

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
	@echo ---------------Instaling YARPPicoloDeviceDriver.lib --------------
	@echo
	$(CP) *.lib $(YARP_ROOT)/lib/$(OS)
	$(CP) *.h 	 $(YARP_ROOT)/include
	+cd ./dd_orig; make install
	
subdirs:
	+cd ./dd_orig; make 




