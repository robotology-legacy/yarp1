ifndef  YARP_ROOT
YARP_ROOT = ../..
endif

MYCODELIB = libYARP_OS2

norm:
	make default && ( cd src-test; make ) && ( cd test; make run )

include $(YARP_ROOT)/conf/Makefile.recursive.template

force:
	make
	cd src-test; make clean ; make
	cd test; $(MAKE) clean
	cd test; $(MAKE) harness.bin
	cd test; $(MAKE) run
	cp test/harness.bin ./cyarp

testing:
	make
	cd test; $(MAKE) run
