#
# Application Makefile   
#

include $(YARP_ROOT)/conf/Makefile.template

STDINCLUDE = -I. -I$(YARP_ROOT)/include -I$(YARP_ROOT)/include/stl -I$(YARP_ROOT)/include/sys -L$(YARP_ROOT)/lib/$(OS)
HWINCLUDE = 
STDLIBS = -lmath.lib -lmodels.lib -los_services.lib

DEBUG=

ifndef DEBUG
DEBUG = -g
endif

ifndef OPTIM
OPTIM = $(CFAST)
endif

PLATFORM=default
PHABOPTS=$(STDINCLUDE) $(HWINCLUDE) $(STDLIBS)

#
#CFLAGS  = -v$(PLATFORM) -3 -mf -w5 $(OPTIM) $(DEBUG) $(PHABOPTS) $(STDINCLUDE) $(HWINCLUDE)
CFLAGS  = -v$(PLATFORM) -3 -mf $(OPTIM) $(DEBUG) $(PHABOPTS) $(STDINCLUDE) $(HWINCLUDE)
CXXFLAGS = $(CFLAGS)
LDFLAGS = -v$(PLATFORM) -3 -mf $(DEBUG) -l Aplib -l phrtlib -l phexlib -l phcontrib -l photon   -l phrender_s $(PHABOPTS)
SDFLAGS = -v$(PLATFORM) -3 -mf $(DEBUG) -l Aplib -l phrtlib -l phexlib -l phcontrib -l photon_s -l phrender_s $(PHABOPTS)

VPATH = ../.

include	../abOfiles
include ../indOfiles

include	../abSfiles
include ../indSfiles

include	../abHfiles
include ../indHfiles

include	../abWfiles

$(CC) = CC

#
# Application Program
#

shr:	$(ABOBJ) $(MYOBJ)
		$(LD) $(SDFLAGS) $(ABOBJ) $(MYOBJ) -M -o egomap_gui
		usemsg egomap_gui ../Usemsg
		phabbind egomap_gui $(ABMOD)

app:	$(ABOBJ) $(MYOBJ)
		$(LD) $(LDFLAGS) $(ABOBJ) $(MYOBJ) -M -o egomap_gui
		usemsg egomap_gui ../Usemsg
		phabbind egomap_gui $(ABMOD)

clean:
		rm -f *.o *.err *.map egomap_gui

install:
		-cp -vnf egomap_gui $(YARP_ROOT)/bin/$(OS)

#
# Headers
#

$(ABOBJ) $(MYOBJ):	$(MYHDR)

#
# Prototype Generation
#

proto:
		approto -p $(ABSRC) $(MYSRC) > proto.h
