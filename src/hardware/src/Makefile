# Device driver global makefile
include $(YARP_ROOT)/conf/Makefile.template

SUBDIRS = picolo 
#galil

all: compile drivers

clean:
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) clean ); done

install: drivers

compile:
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) ); done

drivers: 
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) install ) ; done
	+cd ./common; make install
