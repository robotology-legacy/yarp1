include $(YARP_ROOT)/conf/Makefile.template

ifeq ($(shell uname),QNX)
  CC = cc
  SYSDIR = qnx
else
  ifeq ($(shell uname),Linux)
    CC = g++
    SYSDIR = linux
  else
    CC = g++
    SYSDIR = winnt
  endif
endif

SUBDIRS = src
# $(SYSDIR)

OUT_SYS = obj/$(SYSDIR)
OUT_LIB = lib/$(SYSDIR)

VPATH += $(OUT_SYS)
VPATH += $(OUT_LIB)
VPATH += $(YARP_ROOT)/lib/$(SYSDIR)

OBJS = $(OUT_SYS)/*.o
CODELIB = poking.lib

all: target

w:
	echo -n

target: subtarget local

local: $(CODELIB)

$(CODELIB): subtarget
	$(AR) $(ARFLAGS) $(OUT_LIB)/$(CODELIB) $(OBJS)

install: target subinstall
	$(CP) $(OUT_LIB)/$(CODELIB) $(YARP_ROOT)/lib/$(SYSDIR)
	$(CP) include/*.h $(YARP_ROOT)/include

clean: subclean
	rm -f obj/$(SYSDIR)/*

subtarget :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) target ) ; done

subclean :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) clean ) ; done

subinstall :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) install ) ; done

