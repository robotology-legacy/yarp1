include $(YARP_ROOT)/conf/Makefile.template

SUBDIRS = src
# $(SYSDIR)

OUT_SYS = obj/$(SYSDIR)

VPATH += $(OUT_SYS)
VPATH += $(YARP_ROOT)/lib/$(SYSDIR)

OBJS = $(OUT_SYS)/*.o

CODELIB = utils.lib

all: target

w:
	echo -n

target: subtarget local

local: $(CODELIB)

$(CODELIB): subtarget
	$(AR) $(ARFLAGS) ./lib/$(OS)/$(CODELIB) $(OBJS)

install: target subinstall
	$(CP) ./lib/$(OS)/$(CODELIB) $(YARP_ROOT)/lib/$(SYSDIR)
	$(CP) include/*.h $(YARP_ROOT)/include

clean: subclean
	rm -f $(CODELIB) 
#obj/$(SYSDIR)/*

subtarget :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) target ) ; done

subclean :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) clean ) ; done

subinstall :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) install ) ; done

