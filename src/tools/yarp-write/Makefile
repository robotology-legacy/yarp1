
EXECDIR = ../bin/$(SYSDIR)
EXEC_TARGET = $(EXECDIR)/$(notdir $(PWD))


ifndef  YARP_ROOT
YARP_ROOT = ../../..
endif

include $(YARP_ROOT)/conf/Makefile.template

TARGETS = $(patsubst %.cpp, $(GENDIR)/%.o, $(wildcard *.cpp)) $(patsubst %.c, $(GENDIR)/%.o, $(wildcard *.c))

CFLAGS = -g $(CFAST) $(CDEFS) -I../../libYARP_OS/include -I. -I/usr/include

SUBDIRS =

GENDIR = obj/$(SYSDIR)

VPATH += $(GENDIR)

all: target

target: $(TARGETS) $(EXEC_TARGET)

w:
	@echo -n

$(EXEC_TARGET): $(TARGETS)
	mkdir -p $(EXECDIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) -lACE -L../../libYARP_OS/lib/$(SYSDIR) -lYARP_OS

$(GENDIR)/%.o: %.cpp
	mkdir -p $(GENDIR)
	$(CC) $(CFLAGS) -c $^ -o $@

$(GENDIR)/%.o: %.c
	mkdir -p $(GENDIR)
	$(CC1) $(CFLAGS) -c $^ -o $@

clean: subclean
	rm -f $(TARGETS) $(EXEC_TARGETS)
	rm -f *.o

install: target subinstall
	@echo Doing nothing as yet

subtarget :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) target ) ; done

subclean :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) clean ) ; done

subinstall :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) install ) ; done


