# $Id: Makefile,v 1.6 2003-10-26 22:48:56 babybot Exp $

include $(YARP_ROOT)/conf/Makefile.template

#OBJS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))

CFLAGS = -g -I../include -I../sys_include -I$(YARP_ROOT)/include/ -I$(YARP_ROOT)/src/hardware/include \
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

