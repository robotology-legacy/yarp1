include $(YARP_ROOT)/conf/Makefile.template

SUBDIRS = common eurobot babybot

OUT_SYS = obj/$(SYSDIR)
OUT_LIB = lib/$(SYSDIR)

VPATH += $(OUT_SYS)
VPATH += $(OUT_LIB)
VPATH += $(YARP_ROOT)/lib/$(SYSDIR)

OBJS = $(OUT_SYS)/*.o
CODELIB = MotorCtrl.lib

local: $(CODELIB)

$(CODELIB): subtarget
	$(AR) $(ARFLAGS) $(CODELIB) $(OBJS)
	$(CP) -c $(CODELIB) $(OUT_LIB)/

install: subinstall 
	$(CP) common/*.h $(YARP_ROOT)/include
	$(CP) $(OUT_LIB)/$(CODELIB) $(YARP_ROOT)/lib/$(SYSDIR)
	#$(CP) include/*.h include/*.inl $(YARP_ROOT)/include

clean: subclean
	rm -f obj/$(SYSDIR)/*.o

subtarget :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) target ) ; done

subclean :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) clean ) ; done

subinstall :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) install ) ; done

