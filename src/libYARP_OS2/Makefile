ifndef  YARP_ROOT
YARP_ROOT = ../..
endif

MYCODELIB = libYARP_OS2

all:
	make default
	cd test; $(MAKE) harness.bin
	cp test/harness.bin ./cyarp

trial:
	make default && ( cd src-test; make ) && ( rm /tmp/TRACE.TXT; cd test; export MALLOC_TRACE=/tmp/TRACE.TXT; make clean; make run )
	cp test/harness.bin ./cyarp

trace:
	mtrace /tmp/TRACE.TXT | grep -v "was never alloc"
	@echo "(should expect 0x44 entries on linux for max simultaneous threads created)"

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


add:
	cd include/yarp && ( cvs add *.h || echo -n )
	cd src && ( cvs add *.cpp || echo -n )
	cd src-test && ( cvs add *.cpp *.h || echo -n )

