include $(YARP_ROOT)/conf/Makefile.template

SUBDIRS = LogPolarSmallSDK
# $(SYSDIR)

OUT_SYS = obj/$(SYSDIR)

VPATH += $(OUT_SYS)
VPATH += $(YARP_ROOT)/lib/$(SYSDIR)

OBJS = $(OUT_SYS)/*.o

CODELIB = 

all: logp

w:
	echo -n

logp:	
	cd LogPolarSmallSDK; $(MAKE) $(MAKEFLAGS) all; cd ..

install: subinstall

clean: subclean
	rm -f $(CODELIB) obj/$(SYSDIR)/*

subclean :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) clean ) ; done

subinstall :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) install ) ; done

