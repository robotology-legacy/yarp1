#$Id: Makefile,v 1.3 2004-12-06 10:33:43 babybot Exp $

#
# TODO: missing ACE FLAGS compilation?
#

include $(YARP_ROOT)/conf/Makefile.template

CFLAGS += -I ../dd_orig/include -I $(YARP_ROOT)/include -I $(YARP_ROOT)/include/$(OS) -I ../../../include

all: YARPPicoloDeviceDriver.o

YARPPicoloDeviceDriver.o: YARPPicoloDeviceDriver.h
	$(CC) $(EXTRA_OPTS) $(CFLAGS) -c -o YARPPicoloDeviceDriver.o YARPPicoloDeviceDriver.cpp

clean:
	rm -f *.o
