# Device driver global makefile

ifndef  YARP_ROOT
YARP_ROOT = ../..
endif

include $(YARP_ROOT)/conf/Makefile.template

SUBDIRS = null_grabber src

all: target

target:
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) target ); done

clean:
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) clean ); done

install:
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) install ) ; done

