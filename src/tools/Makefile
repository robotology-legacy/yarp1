
ifndef  YARP_ROOT
YARP_ROOT = ../..
endif

include $(YARP_ROOT)/conf/Makefile.template

SUBDIRS = $(wildcard yarp-*)


default:
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) ) ; done


clean:
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) clean ) ; done

relink:
	rm -f bin/$(SYSDIR)/*
