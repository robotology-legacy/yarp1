#----------------------------------------------------------------------------
#       Makefile,v 1.27 2002/11/16 15:37:16 bala Exp
#
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
#       Local macros
#----------------------------------------------------------------------------

include $(YARP_ROOT)/conf/Makefile.template

TARGETS = $(patsubst %.cpp, %, $(wildcard *.cpp)) 
#TARGETS = exec_test8

# For make depend.
#SRC = $(addsuffix .cpp, $(TARGETS))
#OBJ = $(addsuffix .o, $(TARGETS))

all: 
	@for d in $(TARGETS) ; do \
	( $(MAKE) $(MAKEFLAGS) -f Makefile.ace BIN=$$d ) ; done

clean:
	@for d in $(TARGETS) ; do \
	( $(MAKE) $(MAKEFLAGS) -f Makefile.ace BIN=$$d clean ) ; done

#----------------------------------------------------------------------------
#       Dependencies
#----------------------------------------------------------------------------

# DO NOT DELETE THIS LINE -- g++dep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.


# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
