ifndef  YARP_ROOT
YARP_ROOT = ../..
endif

MYCODELIB = libYARP_OS2

include $(YARP_ROOT)/conf/Makefile.recursive.template

y:
	make
	cd test; $(MAKE) clean
	cd test; $(MAKE) harness.bin
	cp test/harness.bin ./cyarp

testing:
	make
	cd test; $(MAKE) run
