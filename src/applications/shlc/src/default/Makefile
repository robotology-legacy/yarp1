#
# Application Makefile   
#

include $(YARP_ROOT)/conf/Makefile.template

STDINCLUDE = -I. -I$(YARP_ROOT)/include -I$(YARP_ROOT)/include/stl -I$(YARP_ROOT)/include/sys -L$(YARP_ROOT)/lib/$(OS)
HWINCLUDE = -I$(YARP_ROOT)/src/hardware/include -L$(YARP_ROOT)/src/hardware/lib/
STDLIBS = -lmath.lib -lmodels.lib -los_services.lib -lMotorCtrl.lib -lmeid.lib -lisd.lib -lueid.lib -limages.lib -lfakeipl.lib -lpoking.lib

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
CFLAGS  = -v$(PLATFORM) -3 -mf $(OPTIM) $(DEBUG) $(PHABOPTS)
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

ADDOBJ = classes.o secret.o YARPGoodActions.o

#
# Application Program
#

shr:	$(ABOBJ) $(MYOBJ) $(ADDOBJ)
		$(LD) $(SDFLAGS) $(ABOBJ) $(MYOBJ) $(ADDOBJ) -M -o shlc
		usemsg shlc ../Usemsg
		phabbind shlc $(ABMOD)

app:	$(ABOBJ) $(MYOBJ) $(ADDOBJ)
		$(LD) $(LDFLAGS) $(ABOBJ) $(MYOBJ) $(ADDOBJ) -M -o shlc
		usemsg shlc ../Usemsg
		phabbind shlc $(ABMOD)

clean:
		rm -f *.o *.err *.map shlc

install:
		-cp -vnf shlc $(YARP_ROOT)/bin/$(OS)

#
# Headers
#

$(ABOBJ) $(MYOBJ) $(ADDOBJ):	$(MYHDR)

#
# Prototype Generation
#

proto:
		approto -p $(ABSRC) $(MYSRC) > proto.h
