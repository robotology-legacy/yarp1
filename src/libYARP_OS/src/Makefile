
ifndef  YARP_ROOT
YARP_ROOT = ../../..
endif

include $(YARP_ROOT)/conf/Makefile.template

TARGETS = $(patsubst %.cpp, $(GENDIR)/%.o, $(wildcard *.cpp)) $(patsubst %.c, $(GENDIR)/%.o, $(wildcard *.c))

CFLAGS = $(CFAST) $(CDEFS) -I../include/yarp -I../include/yarp_private -I../include/conf -I../include

SUBDIRS =

GENDIR = ../obj/$(SYSDIR)

VPATH += $(GENDIR)

all: target

target: $(TARGETS) subtarget

w:
	@echo -n

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


