#----------------------------------------------------------------------------
#	Makefile,v 1.27 2002/11/16 15:37:16 bala Exp
#
#	Makefile for tests of the miscellaneous ACE performance tests
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
#	Local macros
#----------------------------------------------------------------------------

include $(YARP_ROOT)/conf/Makefile.template
CPPFLAGS += -I$(YARP_ROOT)/include -I$(YARP_ROOT)/include/$(SYSDIR) -I ../utils/include $(CDEFS)

BIN = headcontrol

CDEFS += -D$(ROBOT)

FILES =	HState	\
		HeadBehavior \
		HeadThread

LDLIBS  = $(YARP_ROOT)/lib/$(OS)/libYARP_robot.a \
		  $(YARP_ROOT)/lib/$(OS)/libYARP_dev.a \
		  $(YARP_ROOT)/lib/$(OS)/libYARP_sig.a \
		  $(YARP_ROOT)/lib/$(OS)/libYARP_sig_logpolar.a \
		  $(YARP_ROOT)/lib/$(OS)/libYARP_OS.a \
		  $(YARP_ROOT)/lib/$(OS)/libYARP_math.a

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
