include $(YARP_ROOT)/conf/Makefile.template

SUBDIRS = common $(SYSDIR)

OUT_SYS = obj/$(SYSDIR)

VPATH += $(OUT_SYS)
VPATH += lib/$(SYSDIR)

OBJS_SYS = $(patsubst %.cpp, %.o, $(wildcard common/*.cpp)) $(patsubst %.c, %.o, $(wildcard common/*.c)) $(patsubst %.cpp, %.o, $(wildcard $(SYSDIR)/*.cpp)) $(patsubst %.c, %.o, $(wildcard $(SYSDIR)/*.c))
OBJS_FULL = $(OBJS_SYS)
OBJS = $(addprefix $(OUT_SYS)/, $(notdir $(OBJS_FULL)))

CODELIB = os_services.lib

all: target

target: subtarget local

local: $(CODELIB)

$(CODELIB): $(OBJS)
	echo Making $(CODELIB) for $(OBJS)
	$(AR) $(ARFLAGS) $(CODELIB) $(OBJS)
	mv $(CODELIB) lib/$(SYSDIR)/


install: subinstall
	$(CP) include/*.h $(YARP_ROOT)/include
	$(CP) lib/$(SYSDIR)/* $(YARP_ROOT)/lib/$(SYSDIR)

clean: subclean
	rm -f obj/$(SYSDIR)/*

w:
	@echo -n

subtarget :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) target ) ; done

subclean :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) clean ) ; done

subinstall :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) install ) ; done

