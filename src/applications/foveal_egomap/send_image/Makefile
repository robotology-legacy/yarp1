include $(YARP_ROOT)/conf/Makefile.template

TARGETS = $(patsubst %.cpp, $(GENDIR)/%.o, $(wildcard *.cpp))
EXEC_TARGETS = send_image
CFLAGS = $(CFAST) $(CDEFS) -I$(YARP_ROOT)/include -I$(YARP_ROOT)/include/conf -I$(YARP_ROOT)/include/sys -I/home/cog/include/cpp -I/usr/include
LIBS = -L$(YARP_ROOT)/lib/$(SYSDIR) $(YARP_ROOT)/lib/$(SYSDIR)/poking.lib $(YARP_ROOT)/lib/$(SYSDIR)/math.lib  $(YARP_ROOT)/lib/$(SYSDIR)/images.lib $(YARP_ROOT)/lib/$(SYSDIR)/fakeipl.lib $(YARP_ROOT)/lib/$(SYSDIR)/os_services.lib $(SYSLIB) $(LIBADD)

SUBDIRS =

GENDIR = .

VPATH += $(GENDIR)

all: target

test: target
	@for f in $(EXEC_TARGETS); do \
	echo "# Executing $$f ################################"; \
	$$f; \
	done

target: $(TARGETS) $(EXEC_TARGETS) subtarget

$(GENDIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

$(GENDIR)/$(EXEC_TARGETS): $(TARGETS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean: subclean
	rm -f $(TARGETS) $(EXEC_TARGETS)

install: subinstall
	$(CP) $(EXEC_TARGETS) $(YARP_ROOT)/bin/$(SYSDIR)

subtarget :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) target ) ; done

subclean :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) clean ) ; done

subinstall :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) install ) ; done


