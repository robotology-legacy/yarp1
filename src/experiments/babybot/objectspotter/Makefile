#----------------------------------------------------------------------------
#	Makefile,v 1.27 2002/11/16 15:37:16 bala Exp
#
#	Makefile for tests of the miscellaneous ACE performance tests
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
#	Local macros
#----------------------------------------------------------------------------

include $(YARP_ROOT)/conf/Makefile.template
CPPFLAGS += -I$(YARP_ROOT)/include $(CDEFS)# $(CFAST)

BIN = attn_trackercloned

CDEFS += -D$(ROBOT)

FILES = ImgTrack

LDLIBS  = $(YARP_ROOT)/lib/$(OS)/YARPGalilDeviceDriver.lib	\
		  $(YARP_ROOT)/lib/$(OS)/dmcqnx.lib	\
		  $(YARP_ROOT)/lib/$(OS)/images.lib	\
		  $(YARP_ROOT)/lib/$(OS)/LogPolarSmallSDK.lib	\
		  $(YARP_ROOT)/lib/$(OS)/fakeipl.lib	\
		  $(YARP_ROOT)/lib/$(OS)/os_services.lib	\
		  $(YARP_ROOT)/lib/$(OS)/math.lib	\
		  $(YARP_ROOT)/lib/$(OS)/MotorCtrl.lib	\
		  $(YARP_ROOT)/lib/$(OS)/utils.lib

SRC  = $(addsuffix .cpp,$(FILES))
OBJ  = $(addsuffix .o,$(FILES))
LSRC = $(SRC) $(addsuffix .cpp,$(BIN))

#LDLIBS  +=  $(addprefix .shobj/,$(SHOBJ))

BUILD = $(VBIN)

w:
	@echo please select a robot setup
eurobot:
	@make ROBOT=__QNXEurobot__ all
babybot:
	@make ROBOT=__QNXBabybot__ all
test:
	@make -f Makefile.test ROBOT=__LinuxTest__ all

#----------------------------------------------------------------------------
#	Include macros and targets
#----------------------------------------------------------------------------

include	$(ACE_ROOT)/include/makeinclude/wrapper_macros.GNU
include	$(ACE_ROOT)/include/makeinclude/macros.GNU
include	$(ACE_ROOT)/include/makeinclude/rules.common.GNU
include	$(ACE_ROOT)/include/makeinclude/rules.nonested.GNU
include	$(ACE_ROOT)/include/makeinclude/rules.bin.GNU
include	$(ACE_ROOT)/include/makeinclude/rules.local.GNU

#----------------------------------------------------------------------------
#	Local targets
#----------------------------------------------------------------------------

install:
	cp $(BIN) $(YARP_ROOT)/bin/$(OS)/

#----------------------------------------------------------------------------
#	Dependencies
#----------------------------------------------------------------------------

# DO NOT DELETE THIS LINE -- g++dep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.

# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
