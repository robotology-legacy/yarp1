#
# Application Makefile   
#

ifdef DBG
DBG_RELEASE = -O3 -fomit-frame-pointer
DBG_DEBUG = -O -gdwarf-2
DEBUG=$(DBG_$(DBG))
endif

YARP_ROOT=/home/cbeltran/DEVELOPMENT/yarp
ACE_ROOT=$(YARP_ROOT)/src/libraries/ACE_wrappers

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

PLATFORM=gcc_ntox86
PHABOPTS= 

CC  = QCC
CXX = QCC
LD  = QCC

#CFLAGS  = -V$(PLATFORM) -w5 $(OPTIM) $(DEBUG) $(PHABOPTS) -I.
#CXXFLAGS = $(CFLAGS)
#LDFLAGS = -V$(PLATFORM) -Bstatic $(DEBUG) -l phexlib $(ABLIB) $(PHABOPTS)
#SDFLAGS = -V$(PLATFORM)          $(DEBUG) -l phexlib $(ABLIB) $(PHABOPTS)

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

#SRC    = $(addsuffix .cpp,$(FILES))
#OBJ    = $(addsuffix .o,$(FILES))
#LSRC    = $(SRC) $(addsuffix .cpp,$(BIN))
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
		#usemsg test ../Usemsg
		#phabbind test $(ABMOD)

app:
	make
		make ttt
		#usemsg test ../Usemsg
		#phabbind test $(ABMOD)

clean:
	#make clean
		rm -f *.o *.err *.map abmain
		rm -f -r .obj/

install:
	#-cp -vnf test /usr/photon/bin
	cp ./abmain $(YARP_ROOT)/bin/$(OS)/camview
##################################################

# IF YOU PUT ANYTHING HERE IT WILL GO AWAY

########################################à
#
# Headers
#

$(ABOBJ) $(MYOBJ):	$(MYHDR)

#
# Prototype Generation
#

proto:
	approto -p $(ABSRC) $(MYSRC) > proto.h
