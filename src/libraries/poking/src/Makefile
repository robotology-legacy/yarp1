include $(YARP_ROOT)/conf/Makefile.template

VVINC=/usr/lib/ViaVoice/include

TARGETS = $(patsubst %.cpp, $(GENDIR)/%.o, $(wildcard *.cpp)) $(patsubst %.c, $(GENDIR)/%.o, $(wildcard *.c))
#CFLAGS = -g3 $(CDEFS) -I../include -I$(YARP_ROOT)/include -I$(YARP_ROOT)/include/stl -I$(YARP_ROOT)/include/sys
CFLAGS = $(CFAST) -I. $(CDEFS) -I$(VVINC) -I../include -I$(YARP_ROOT)/include -I$(YARP_ROOT)/include/stl -I$(YARP_ROOT)/include/sys

SUBDIRS =

GENDIR = ../obj/$(SYSDIR)

VPATH += $(GENDIR)

all: target

target: $(TARGETS) subtarget

$(GENDIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

$(GENDIR)/%.o: %.c
	$(CC1) $(CFLAGS) -c $^ -o $@

#$(GENDIR)/%: %.o
#	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

w:
	@echo -n

clean: subclean
	rm -f $(TARGETS) $(EXEC_TARGETS)
	rm -f *.o

install: subinstall
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


