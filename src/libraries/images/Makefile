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

SUBDIRS = src temporary
# $(SYSDIR)

OUT_SYS = obj/$(SYSDIR)

VPATH += $(OUT_SYS)
VPATH += $(YARP_ROOT)/lib/$(SYSDIR)

OBJS = $(OUT_SYS)/*.o

CODELIB = images.lib

all: target

w:
	echo -n

target: subtarget local

local: $(CODELIB)

$(CODELIB): subtarget
	$(AR) $(ARFLAGS) $(CODELIB) $(OBJS)

install: target subinstall
	$(CP) $(CODELIB) $(YARP_ROOT)/lib/$(SYSDIR)
	$(CP) include/*.h $(YARP_ROOT)/include
	$(CP) temporary/*.h $(YARP_ROOT)/include/temporary
	$(CP) temporary/conf/*.h $(YARP_ROOT)/include/conf

clean: subclean
	rm -f $(CODELIB) obj/$(SYSDIR)/*

subtarget :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) target ) ; done

subclean :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) clean ) ; done

subinstall :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) install ) ; done

