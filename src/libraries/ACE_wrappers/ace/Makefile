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
	@$(MAKE) -f Makefile.dirs $@

# Invoke Makefile.ace first to build libACE, and then
# invoke Makefile.dirs to recurse through subdirectories
all:
	@$(MAKE) -f Makefile.ace
	@$(MAKE) -f Makefile.dirs
