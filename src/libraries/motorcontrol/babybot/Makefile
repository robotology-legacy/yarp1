# $Id: Makefile,v 1.7 2004-02-04 15:09:26 beltran Exp $

include $(YARP_ROOT)/conf/Makefile.template

#OBJS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))

CFLAGS = $(CFAST) $(CDEFS) -I../include -I../sys_include -I$(YARP_ROOT)/include/ -I$(YARP_ROOT)/src/hardware/include \
-I../common

LIBS = $(YARP_ROOT)/lib/$(SYSDIR)/os_services.lib $(YARP_ROOT)/bin/$(SYSDIR)/libACE.so \
$(YARP_ROOT)/lib/$(OS)/YARPGalilDeviceDriver.lib $(YARP_ROOT)/lib/$(OS)/dmcqnx.lib	 

CODELIB = 

OUTDIR = ../obj/$(SYSDIR)

# VPATH += $(OUTDIR)

#%.o: %.cpp
#	$(CC) $(CFLAGS) -c $^ -o $@
#	cp $@ $(OUTDIR)

local: 

all: target

target: local

clean:
	
install:
	cp *.h $(YARP_ROOT)/include

