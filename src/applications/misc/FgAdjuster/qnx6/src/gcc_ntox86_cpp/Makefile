#
# Application Makefile
#

ifdef DBG
DBG_RELEASE = -O3 -fomit-frame-pointer
DBG_DEBUG = -O -gdwarf-2
DEBUG=$(DBG_$(DBG))
endif

include $(YARP_ROOT)/conf/Makefile.template

ifndef DEBUG
DEBUG = -g
endif

ifndef OPTIM
OPTIM = -O
endif

include	../abOfiles
include ../indOfiles

include	../abSfiles
include ../indSfiles

include	../abHfiles
include ../indHfiles

include	../abWfiles
include	../abLfiles

PLATFORM=gcc_ntox86_cpp
PHABOPTS= 
APPROTOPATH=/usr/photon/appbuilder/

CC  = QCC
CXX = QCC
LD  = QCC

CFLAGS  =  -w5 $(OPTIM) $(DEBUG) $(PHABOPTS) -I.
CXXFLAGS = $(CFLAGS)
LDFLAGS =  $(DEBUG) -l phexlib $(ABLIB) $(PHABOPTS)
SDFLAGS =         $(DEBUG) -l phexlib $(ABLIB) $(PHABOPTS)

CPPFLAGS     += -I$(YARP_ROOT)/include $(CDEFS)

VPATH = ..

BIN     = abmain

FILES =

LDLIBS  =	$(YARP_ROOT)/lib/$(OS)/MotorCtrl.lib	\
			$(YARP_ROOT)/lib/$(OS)/utils.lib	\
			$(YARP_ROOT)/lib/$(OS)/images.lib	\
			$(YARP_ROOT)/lib/$(OS)/LogPolarSmallSDK.lib	\
			$(YARP_ROOT)/lib/$(OS)/fakeipl.lib	\
			$(YARP_ROOT)/lib/$(OS)/os_services.lib	\
			$(YARP_ROOT)/lib/$(OS)/math.lib	

SRC    = $(ABSRC) $(MYSRC)
OBJ    = $(ABOBJ) $(MYOBJ)
LSRC    = $(SRC) $(addsuffix .cpp,$(BIN))


BUILD = $(VBIN)

#----------------------------------------------------------------------------
#	Include macros and targets
#----------------------------------------------------------------------------

include	$(ACE_ROOT)/include/makeinclude/wrapper_macros.GNU
include	$(ACE_ROOT)/include/makeinclude/macros.GNU
include	$(ACE_ROOT)/include/makeinclude/rules.common.GNU
include	$(ACE_ROOT)/include/makeinclude/rules.nonested.GNU
include	$(ACE_ROOT)/include/makeinclude/rules.bin.GNU
include	$(ACE_ROOT)/include/makeinclude/rules.local.GNU

#
# Application Program
#
ttt:
	usemsg abmain ../Usemsg
	/usr/photon/appbuilder/phabbind abmain $(ABMOD)

shr:	
		make
		make ttt
app:
		make
		make ttt
clean:
		rm -f *.o *.err *.map abmain
		rm -f -r .obj/
install:
	cp abmain $(YARP_ROOT)/bin/$(OS)/fgadjuster
##################################################

# IF YOU PUT ANYTHING HERE IT WILL GO AWAY

########################################�
#
# Headers
#

$(ABOBJ) $(MYOBJ):	$(MYHDR)

#
# Prototype Generation
#
proto:
	rm -f proto.h
	$(APPROTOPATH)approto -p $(ABSRC) $(MYSRC) > proto.h
