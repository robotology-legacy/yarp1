# Device driver global makefile

ifndef  YARP_ROOT
YARP_ROOT = ../..
endif

include $(YARP_ROOT)/conf/Makefile.template

SUBDIRS = galil null_grabber picolo src

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

