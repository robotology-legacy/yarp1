#
# Application Makefile   
#

include $(YARP_ROOT)/conf/Makefile.template

ifndef DEBUG
DEBUG = -g1
endif

ifndef OPTIM
OPTIM = $(CFAST)
endif

PLATFORM=default
#MOPTS= -I/home/scaz/ai/vision/include -L/home/scaz/ai/vision/lib -lmht
LINKOBJS=-L$(YARP_ROOT)/lib/qnx images.lib fakeipl.lib os_services.lib
MOPTS= -I$(YARP_ROOT)/include -I$(YARP_ROOT)/include/sys
PHABOPTS= -I$(YARP_ROOT)/src/hardware/include -I$(YARP_ROOT)/src/hardware/include/cpp -L$(YARP_ROOT)/src/hardware/lib $(MOPTS)

#
CFLAGS  = -g -v$(PLATFORM) -3 -mf -w1 $(OPTIM) $(DEBUG) $(PHABOPTS) 
CXXFLAGS = $(CFLAGS)
LDFLAGS = -v$(PLATFORM) -3 -mf $(DEBUG) -l Aplib -l phrtlib -l phexlib -l phcontrib -l photon   -l phrender_s $(PHABOPTS)
SDFLAGS = -v$(PLATFORM) -3 -mf $(DEBUG) -l Aplib -l phrtlib -l phexlib -l phcontrib -l photon_s -l phrender_s $(PHABOPTS)

VPATH = ../.

.SUFFIXES:
.SUFFIXES:  .o .c .a .cpp .cc

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
CC = cc

%.o: %.cc
		$(CC) $(CFLAGS) -c $^ -o $@

shr:	$(ABOBJ) $(MYOBJ)
		$(LD) $(SDFLAGS) $(ABOBJ) $(MYOBJ) $(LINKOBJS) -M -o cogcam
		usemsg cogcam ../Usemsg
		phabbind cogcam $(ABMOD)

app:	$(ABOBJ) $(MYOBJ)
		$(LD) $(LDFLAGS) $(ABOBJ) $(MYOBJ) $(LINKOBJS) -M -o cogcam
		usemsg cogcam ../Usemsg
		phabbind cogcam $(ABMOD)

clean:
		rm -f *.o *.err *.map cogcam

install:
#		-cp -vnf cogcam /usr/bin/photon
		$(CP) cogcam $(YARP_ROOT)/bin/$(OS)/cogcam
		
#
# Headers
#

$(ABOBJ) $(MYOBJ):	$(MYHDR)

#
# Prototype Generation
#

proto:
		approto -p $(ABSRC) $(MYSRC) > proto.h
