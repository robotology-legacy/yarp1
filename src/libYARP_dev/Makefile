# Device driver global makefile

ifndef  YARP_ROOT
YARP_ROOT = ../..
endif

include $(YARP_ROOT)/conf/Makefile.template

SUBDIRS = picolo galil 

all: compile drivers

clean:
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) clean ); done

install: drivers

compile:
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) ); done

drivers: 
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) install ) ; done
