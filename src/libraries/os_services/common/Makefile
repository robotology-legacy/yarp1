include $(YARP_ROOT)/conf/Makefile.template

#ifeq ($(shell uname),QNX)
#  CC = cc
#  CC1 = $(CC)
   CC1 = gcc
#  CDEFS = -D__QNX__ -D__QNX4__
#  SYSDIR = qnx
#  SYSLIB = 
#else
#  ifeq ($(shell uname),Linux)
#    CC = g++
#    CC1 = gcc
#    CDEFS = -D__LINUX__
#    SYSDIR = linux
#    SYSLIB = -lpthread
#  else
#    CC = g++ 
#    CC1 = gcc 
#    CDEFS = -D__WIN__
#    SYSDIR = winnt
#    SYSLIB = 
#  endif
#endif

TARGETS = $(patsubst %.cpp, $(GENDIR)/%.o, $(wildcard *.cpp)) $(patsubst %.c, $(GENDIR)/%.o, $(wildcard *.c))
CFLAGS = -g $(CFAST) $(CDEFS) -I../include -I../sys_include -I/home/cog/include/cpp -I/usr/include
#cpp is just there for the stl
#LIBS = ../lib/$(SYSDIR)/os_services.lib $(SYSLIB)

SUBDIRS =

GENDIR = ../obj/$(SYSDIR)

VPATH += $(GENDIR)

all: target

target: $(TARGETS) subtarget

w:
	@echo -n

$(GENDIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

$(GENDIR)/%.o: %.c
	$(CC1) $(CFLAGS) -c $^ -o $@

#$(GENDIR)/%: %.o
#	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

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


