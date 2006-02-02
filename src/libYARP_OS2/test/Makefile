

ifndef  YARP_ROOT
YARP_ROOT = ../../..
endif

newdefault: yarp

include $(YARP_ROOT)/conf/Makefile.tool.template

INCS += -I../src-test

VPATH += $(EXECDIR)

# unfortunately default target won't work - links to YARP1 library
yarp: $(TARGETS)
	 mkdir -p $(EXECDIR)
	 $(CC) $(CFLAGS) $^ -o $(EXECDIR)/$@ -L$(YARP_ROOT)/lib/$(SYSDIR) $(MYLIBS) -lYARP_OS2_test -lYARP_OS2 -lACE $(LIBADD)

