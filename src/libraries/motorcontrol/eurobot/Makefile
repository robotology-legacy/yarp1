# $Id: Makefile,v 1.5 2003-08-01 08:56:38 beltran Exp $

include $(YARP_ROOT)/conf/Makefile.template

#OBJS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
#OBJS = YARPGALILOnEurobotHeadAdapter.o

CFLAGS = -g -I../include -I../sys_include -I$(YARP_ROOT)/include/ -I$(YARP_ROOT)/src/hardware/include \
-I../common

LIBS = $(YARP_ROOT)/lib/$(SYSDIR)/os_services.lib $(YARP_ROOT)/bin/$(SYSDIR)/libACE.so \
$(YARP_ROOT)/lib/$(OS)/YARPGalilDeviceDriver.lib $(YARP_ROOT)/lib/$(OS)/dmcqnx.lib	 

SUBDIRS =

CODELIB = 

OUTDIR = ../obj/$(SYSDIR)

# VPATH += $(OUTDIR)

#%.o: %.cpp
#	$(CC) $(CFLAGS) -c $^ -o $@
#	cp $@ $(OUTDIR)

#YARPGALILOnEurobotHeadAdapter.o: YARPGALILOnEurobotHeadAdapter.h
#	$(CC) $(CFLAGS) -c $^ -o $@

local: cardtest.cpp
	$(CC) $(CFLAGS)  cardtest.cpp -o cardtest $(LIBS)
	$(CC) $(CFLAGS)  -c YARPEurobotArm.cpp
	$(CP) -c YARPEurobotArm.o $(OUTDIR)/

all: target

target: local

local: $(OBJS)

w:
	@echo -n

clean:
	rm -f $(OUTDIR)/*.o *.o $(CODELIB) 
	rm -f cardtest

install:
	cp *.h $(YARP_ROOT)/include

