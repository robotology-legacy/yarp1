# $Id: Makefile,v 1.2 2003-09-12 16:57:05 beltran Exp $

include $(YARP_ROOT)/conf/Makefile.template

OBJS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))

CFLAGS = -g -I../include -I../sys_include -I$(YARP_ROOT)/include/ -I$(YARP_ROOT)/src/hardware/include \
-I../common

#LIBS = $(YARP_ROOT)/lib/$(SYSDIR)/os_services.lib $(YARP_ROOT)/bin/$(SYSDIR)/libACE.so \
#$(YARP_ROOT)/lib/$(OS)/YARPGalilDeviceDriver.lib $(YARP_ROOT)/lib/$(OS)/dmcqnx.lib $(YARP_ROOT)/lib/$(OS)/MotorCtrl.lib 

OUTDIR = ../obj/$(SYSDIR)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
	$(CP) -c $@ $(OUTDIR)

target: local install

local: $(OBJS)

w:
	@echo -n

clean:
	rm -f $(OUTDIR)/*.o *.o $(CODELIB) 

install:
	cp *.h $(YARP_ROOT)/include

