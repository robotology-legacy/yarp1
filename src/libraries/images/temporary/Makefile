ifeq ($(shell uname),QNX)
  CC = cc
  CC1 = $(CC)
  CDEFS = -D__QNX__ -D__QNX4__
  SYSDIR = qnx
  SYSLIB = 
else
  ifeq ($(shell uname),Linux)
    CC = g++
    CC1 = gcc
    CDEFS = -D__LINUX__
    SYSDIR = linux
    SYSLIB = -lpthread
  else
    CC = g++ 
    CC1 = gcc 
    CDEFS = -D__WIN__
    SYSDIR = winnt
    SYSLIB = 
  endif
endif

TARGETS = $(patsubst %.cpp, $(GENDIR)/%.o, $(wildcard *.cpp)) $(patsubst %.c, $(GENDIR)/%.o, $(wildcard *.c))
#CFLAGS = -g3 $(CDEFS) -I. -I../include -I$(YARP_ROOT)/include -I$(YARP_ROOT)/include/stl -I$(YARP_ROOT)/include/sys
CFLAGS = -oxt $(CDEFS) -I. -I../include -I$(YARP_ROOT)/include -I$(YARP_ROOT)/include/stl -I$(YARP_ROOT)/include/sys
MOREFLAGS = -I$(YARPED_ROOT)/include -I$(YARPED_ROOT)/include/cpp -I$(YARP_ROOT)/src/hardware/include/cpp

SUBDIRS =

GENDIR = ../obj/$(SYSDIR)

VPATH += $(GENDIR)

all: target

target: $(TARGETS) subtarget

$(GENDIR)/%.o: %.cpp
	$(CC) $(CFLAGS) $(MOREFLAGS) -c $^ -o $@

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


