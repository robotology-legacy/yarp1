include $(YARP_ROOT)/conf/Makefile.template

TARGETS = $(patsubst %.cpp, $(GENDIR)/%.o, $(wildcard *.cpp))
EXEC_TARGETS = ybc
# $(patsubst %.cpp, $(GENDIR)/%, $(wildcard *.cpp))
CFLAGS = $(CDEFS) $(CFAST) -I$(YARP_ROOT)/include -I/home/cog/include/cpp -I/usr/include
LIBS = $(YARP_ROOT)/lib/$(SYSDIR)/os_services.lib $(SYSLIB)

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

$(GENDIR)/%: %.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

clean: subclean
	rm -f $(TARGETS) $(EXEC_TARGETS)

install: subinstall
	$(CP) $(EXEC_TARGETS) $(YARP_ROOT)/bin/$(OS)

subtarget :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) target ) ; done

subclean :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) clean ) ; done

subinstall :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) install ) ; done


