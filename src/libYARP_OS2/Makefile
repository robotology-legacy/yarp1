ifndef  YARP_ROOT
YARP_ROOT = ../..
endif


include $(YARP_ROOT)/conf/Makefile.recursive.template

SUBDIRS += src-test test

# generate doxygen comments
dox:
	doxygen
	doxygen Doxyfile.part

user:
	doxygen Doxyfile.part

alldoc:
	make dox
	make user
	cd doc/spec; make pdf

add:
	cd include/yarp && ( cvs add *.h || echo -n )
	cd include/yarp/os && ( cvs add *.h || echo -n )
	cd src && ( cvs add *.cpp || echo -n )
	cd src-test && ( cvs add *.cpp *.h || echo -n )

regression:
	make && ./bin/$(SYSDIR)/yarp regression
