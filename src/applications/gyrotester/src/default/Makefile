#
# Application Makefile   
#

include $(YARP_ROOT)/conf/Makefile.template

ifndef DEBUG
DEBUG = -g
endif

ifndef OPTIM
OPTIM = $(CFAST)
endif

PLATFORM=default
PHABOPTS=
MYOPTS=-I$(YARP_ROOT)/src/hardware/include -L$(YARP_ROOT)/src/hardware/lib -lisd

#
CFLAGS  = -v$(PLATFORM) -3 -mf -w5 $(OPTIM) $(DEBUG) $(MYOPTS) $(PHABOPTS)
CXXFLAGS = $(CFLAGS)
LDFLAGS = -v$(PLATFORM) -3 -mf $(DEBUG) -l Aplib -l phrtlib -l phexlib -l phcontrib -l photon   -l phrender_s $(MYOPTS) $(PHABOPTS)
SDFLAGS = -v$(PLATFORM) -3 -mf $(DEBUG) -l Aplib -l phrtlib -l phexlib -l phcontrib -l photon_s -l phrender_s $(MYOPTS) $(PHABOPTS)

VPATH = ../.

include	../abOfiles
include ../indOfiles

include	../abSfiles
include ../indSfiles

include	../abHfiles
include ../indHfiles

include	../abWfiles

#
# Application Program
#

shr:	$(ABOBJ) $(MYOBJ)
		$(LD) $(SDFLAGS) $(ABOBJ) $(MYOBJ) -M -o gyrotester
		usemsg gyrotester ../Usemsg
		phabbind gyrotester $(ABMOD)

app:	$(ABOBJ) $(MYOBJ)
		$(LD) $(LDFLAGS) $(ABOBJ) $(MYOBJ) -M -o gyrotester
		usemsg gyrotester ../Usemsg
		phabbind gyrotester $(ABMOD)

clean:
		rm -f *.o *.err *.map gyrotester

install:
		$(CP) gyrotester $(YARP_ROOT)/bin/$(OS)/gyrotester

#
# Headers
#

$(ABOBJ) $(MYOBJ):	$(MYHDR)

#
# Prototype Generation
#

proto:
		approto -p $(ABSRC) $(MYSRC) > proto.h
