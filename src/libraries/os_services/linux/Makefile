include $(YARP_ROOT)/conf/Makefile.template

SYSDIR = linux

OBJS = $(patsubst %.cpp, %.o, $(wildcard *.cpp)) $(patsubst %.c, %.o, $(wildcard *.c))
CC = g++ $(CFAST)
CC1 = gcc $(CFAST)
CFLAGS = -I../include -I/usr/include

OUTDIR = ../obj/$(SYSDIR)
VPATH += $(OUTDIR)

SUBDIRS =

#CODELIB = os_services.lib

all: target

target: subtarget $(OBJS)

#local: $(CODELIB)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $(OUTDIR)/$@

%.o: %.c
	$(CC1) $(CFLAGS) -c $^ -o $(OUTDIR)/$@

$(CODELIB): $(OBJS)
	$(AR) crsv $(CODELIB) $(OBJS)
	cp $(CODELIB) ../lib/$(SYSDIR)

dummy: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -lpthread

w:
	@echo -n

clean: subclean
	rm -f *.o $(CODELIB) 

install: subinstall target
#	@echo Doing nothing as yet

subtarget :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) target ) ; done

subclean :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) clean ) ; done

subinstall :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) install ) ; done


