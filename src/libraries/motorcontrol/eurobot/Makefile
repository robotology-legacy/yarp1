# $Id: Makefile,v 1.3 2003-06-20 09:53:36 beltran Exp $

include $(YARP_ROOT)/conf/Makefile.template

#SYSDIR = 

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

cardtest: cardtest.cpp
	$(CC) $(CFLAGS)  cardtest.cpp -o cardtest $(LIBS)

all: target

target: local subtarget

local: $(OBJS)

w:
	@echo -n

clean: subclean
	rm -f $(OUTDIR)/*.o *.o $(CODELIB) 
	rm cardtest

install:
	cp *.h $(YARP_ROOT)/include

