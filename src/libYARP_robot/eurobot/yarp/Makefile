# $Id: Makefile,v 1.1 2004-07-28 17:17:35 babybot Exp $

include $(YARP_ROOT)/conf/Makefile.template

#OBJS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
#OBJS = YARPGALILOnEurobotHeadAdapter.o

CFLAGS = $(CFAST) $(CDEFS) -I../include -I../sys_include -I$(YARP_ROOT)/include/ -I$(YARP_ROOT)/src/hardware/include \
-I../common

LIBS = $(YARP_ROOT)/lib/$(SYSDIR)/os_services.lib $(YARP_ROOT)/bin/$(SYSDIR)/libACE.so \
$(YARP_ROOT)/lib/$(OS)/YARPGalilDeviceDriver.lib $(YARP_ROOT)/lib/$(OS)/dmcqnx.lib $(YARP_ROOT)/lib/$(OS)/MotorCtrl.lib	\
$(YARP_ROOT)/lib/$(OS)/utils.lib 

SUBDIRS =

CODELIB = 

OUTDIR = ../obj/$(SYSDIR)

# VPATH += $(OUTDIR)

#%.o: %.cpp
#	$(CC) $(CFLAGS) -c $^ -o $@
#	cp $@ $(OUTDIR)

#YARPGALILOnEurobotHeadAdapter.o: YARPGALILOnEurobotHeadAdapter.h
#	$(CC) $(CFLAGS) -c $^ -o $@

test: cardtest.cpp
	$(CC) $(CFLAGS)  cardtest.cpp -o cardtest $(LIBS)
	$(CC) $(CFLAGS)  armtest.cpp -o armtest $(LIBS)

all: target

target: local

local: $(OBJS)
	$(CC) $(CFLAGS)  -c YARPEurobotArm.cpp
	$(CP) -c YARPEurobotArm.o $(OUTDIR)/
	$(CC) $(CFLAGS)  -c YARPEurobotHeadKin.cpp
	$(CP) -c YARPEurobotHeadKin.o $(OUTDIR)/

w:
	@echo -n

clean:
	rm -f $(OUTDIR)/*.o *.o $(CODELIB) 
	rm -f cardtest

install:
	cp *.h $(YARP_ROOT)/include
