# Device driver global makefile
include $(YARP_ROOT)/conf/Makefile.template

SUBDIRS = FgAdjuster 

all: compile

clean:
	@for d in $(SUBDIRS) ; do \
		( cd $$d/$(OS) ; $(MAKE) $(MAKEFLAGS) clean ); done

compile:
	@for d in $(SUBDIRS) ; do \
		( cd $$d/$(OS) ; $(MAKE) $(MAKEFLAGS) ); done

install: 
	@for d in $(SUBDIRS) ; do \
		( cd $$d/$(OS) ; $(MAKE) $(MAKEFLAGS) install ) ; done
