#----------------------------------------------------------------------------
#       Makefile,v 4.315 2002/08/31 01:34:30 crodrigu Exp
#
#       Makefile for the ACE library
#----------------------------------------------------------------------------

include $(YARP_ROOT)/conf/Makefile.template

# This default rule is here so invoking make realclean will do
# work when recursing through subdirectories
.DEFAULT:
	@$(MAKE) -f Makefile.ace $@
#	@$(MAKE) -f Makefile.dirs $@

# Invoke Makefile.ace first to build libACE, and then
# invoke Makefile.dirs to recurse through subdirectories
all:
	@$(MAKE) -f Makefile.ace

ace_only:
	@$(MAKE) -f Makefile.ace

yarpize:
	cp libACE.so.5.3.3 $(YARP_ROOT)/bin/$(OS)/
	cp *.h $(YARP_ROOT)/include/ace/
	cp *.i $(YARP_ROOT)/include/ace/
	cp *.cpp $(YARP_ROOT)/include/ace/
	cp *.inl $(YARP_ROOT)/include/ace/
	ln -s -f $(YARP_ROOT)/bin/$(OS)/libACE.so.5.3.3 $(YARP_ROOT)/bin/$(OS)/libACE.so

install: yarpize
