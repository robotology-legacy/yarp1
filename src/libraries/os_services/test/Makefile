#----------------------------------------------------------------------------
#       Makefile,v 1.27 2002/11/16 15:37:16 bala Exp
#
#       Makefile for tests of the miscellaneous ACE performance tests
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
#       Local macros
#----------------------------------------------------------------------------

include $(YARP_ROOT)/conf/Makefile.template
CPPFLAGS     += -I$(YARP_ROOT)/include $(CDEFS)

#BIN     = exec_test1
LDLIBS  = $(MATHLIB) $(YARP_ROOT)/lib/$(SYSDIR)/os_services.lib

BUILD = $(VBIN)


#----------------------------------------------------------------------------
#       Include macros and targets
#----------------------------------------------------------------------------

include $(ACE_ROOT)/include/makeinclude/wrapper_macros.GNU
include $(ACE_ROOT)/include/makeinclude/macros.GNU
include $(ACE_ROOT)/include/makeinclude/rules.common.GNU
include $(ACE_ROOT)/include/makeinclude/rules.nonested.GNU
include $(ACE_ROOT)/include/makeinclude/rules.bin.GNU
include $(ACE_ROOT)/include/makeinclude/rules.local.GNU

GENDIR = obj/$(SYSDIR)

TARGETS = $(patsubst %.cpp, $(GENDIR)/%.o, $(wildcard *.cpp))
EXEC_TARGETS = $(patsubst %.cpp, $(GENDIR)/%, $(wildcard exec_*.cpp))

BIN	= exec_test1

# For make depend.
SRC = $(addsuffix .cpp, $(BIN))


VPATH += $(GENDIR)

all: target

test: target
	@for f in $(EXEC_TARGETS); do \
	echo "# Executing $$f ################################"; \
	$$f; \
	done

target: $(TARGETS) $(EXEC_TARGETS) subtarget

#$(GENDIR)/%.o: %.cpp
#	$(CC) $(CFLAGS) -c $^ -o $@

$(GENDIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ 

$(GENDIR)/%: %.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

clean: subclean
	rm -f $(TARGETS) $(EXEC_TARGETS)

install: subinstall
	@echo Doing nothing as yet

subtarget :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) target ) ; done

subclean :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) clean ) ; done

subinstall :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) install ) ; done


