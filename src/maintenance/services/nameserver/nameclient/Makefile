#----------------------------------------------------------------------------
#	Makefile,v 1.27 2002/11/16 15:37:16 bala Exp
#
#	Makefile for tests of the miscellaneous ACE performance tests
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
#	Local macros
#----------------------------------------------------------------------------

include $(YARP_ROOT)/conf/Makefile.template
CPPFLAGS     += -I$(YARP_ROOT)/include $(CDEFS) $(CFAST)

BIN     = main
TBIN	= nc
	
FILES 	= 

LIBPATH	= $(YARP_ROOT)/lib/$(OS)
LDLIBS  = $(MATHLIB) $(LIBPATH)/os_services.lib $(LIBPATH)/utils.lib

LSRC    = $(addsuffix .cpp,$(FILES))
LOBJ    = $(addsuffix .o,$(FILES))
SHOBJ   = $(addsuffix .so,$(FILES))

#LDLIBS  =  $(addprefix .shobj/,$(SHOBJ))
LDLIBS  +=  $(addprefix .obj/,$(LOBJ))


#VLDLIBS = $(LDLIBS:%=%$(VAR))

BUILD = $(VBIN)

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

install:
	cp $(BIN) $(YARP_ROOT)/bin/$(OS)/$(TBIN)

#----------------------------------------------------------------------------
#	Dependencies
#----------------------------------------------------------------------------

# DO NOT DELETE THIS LINE -- g++dep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.

.obj/YARPNameService.o	:	.obj/YARPNameServer.o	.obj/LocalNameServer.o
.obj/YARPNameServer.o .shobj/YARPNameServer.so :  YARPNameServer.cpp YARPNameServer.h
.obj/LocalNameServer.o .shobj/YARPNameServer.so : LocalNameServer.cpp LocalNameServer.h




