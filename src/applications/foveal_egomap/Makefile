include $(YARP_ROOT)/conf/Makefile.template

ifeq ($(shell uname),QNX)
	MEI = $(YARP_ROOT)/src/hardware/lib/meid.lib
else
	MEI = 
endif

TARGETS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
EXEC_TARGETS = foveal_egomap
CFLAGS = $(CFAST) $(CDEFS) -I$(YARP_ROOT)/include -I$(YARP_ROOT)/include/conf -I$(YARP_ROOT)/include/sys -I$(YARP_ROOT)/include/stl -I/usr/include
LIBS =  $(YARP_ROOT)/lib/$(SYSDIR)/math.lib -L$(YARP_ROOT)/lib/$(SYSDIR) $(YARP_ROOT)/lib/$(SYSDIR)/MotorCtrl.lib $(YARP_ROOT)/lib/$(SYSDIR)/poking.lib  $(YARP_ROOT)/lib/$(SYSDIR)/images.lib $(YARP_ROOT)/lib/$(SYSDIR)/fakeipl.lib $(YARP_ROOT)/lib/$(SYSDIR)/os_services.lib $(SYSLIB) $(LIBADD)

SUBDIRS =

all: target

test: target
	@for f in $(EXEC_TARGETS); do \
	echo "# Executing $$f ################################"; \
	$$f; \
	done

target: $(TARGETS) $(EXEC_TARGETS) subtarget

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

$(EXEC_TARGETS): $(TARGETS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean: subclean
	rm -f $(TARGETS) $(EXEC_TARGETS)

install: subinstall
	cp -t $(EXEC_TARGETS) $(YARP_ROOT)/bin/$(SYSDIR)

subtarget :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) target ) ; done

subclean :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) clean ) ; done

subinstall :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) install ) ; done


