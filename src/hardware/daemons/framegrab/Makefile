#----------------------------------------------------------------------------
#	Makefile,v 1.27 2002/11/16 15:37:16 bala Exp
#
#	Makefile for tests of the miscellaneous ACE performance tests
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
#	Local macros
#----------------------------------------------------------------------------

include $(YARP_ROOT)/conf/Makefile.template
CPPFLAGS     += -I$(YARP_ROOT)/include $(CDEFS)

BIN     = main

CDEFS += -D$(ROBOT)

FILES 	=
	  
LDLIBS  =	$(YARP_ROOT)/lib/$(SYSDIR)/YARPPicoloDeviceDriver.lib	\
		$(YARP_ROOT)/lib/$(OS)/bttvx.lib	\
		$(MATHLIB) 	\
		$(YARP_ROOT)/lib/$(SYSDIR)/images.lib	\
		$(YARP_ROOT)/lib/$(OS)/LogPolarSmallSDK.lib	\
		$(YARP_ROOT)/lib/$(SYSDIR)/fakeipl.lib	\
		$(YARP_ROOT)/lib/$(OS)/os_services.lib
	
LSRC    = $(addsuffix .cpp,$(FILES))
LOBJ    = $(addsuffix .o,$(FILES))
SHOBJ   = $(addsuffix .so,$(FILES))

LDLIBS  +=  $(addprefix .shobj/,$(SHOBJ))

VLDLIBS = $(LDLIBS:%=%$(VAR))

BUILD = $(VBIN)

w:
	@echo please select a robot setup

eurobot:
	@make ROBOT=__QNXEurobot__ all

babybot:
	@make ROBOT=__QNXBabybot__ all

test:
	@make -f Makefile.test ROBOT=__LinuxTest__ all

install:
	$(CP) main $(YARP_ROOT)/bin/$(OS)/framegrab

# For make depend.
SRC = $(addsuffix .cpp, $(BIN))

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

#----------------------------------------------------------------------------
#	Dependencies
#----------------------------------------------------------------------------

# DO NOT DELETE THIS LINE -- g++dep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.

main.o: main.cpp

# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
