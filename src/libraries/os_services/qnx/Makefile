include $(YARP_ROOT)/conf/Makefile.template

SYSDIR = qnx

OBJS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
CC = cc
CFLAGS = -I../include -I/usr/include -I/home/cog/include/cpp -I../sys_include

SUBDIRS =

CODELIB = os_services.lib

OUTDIR = ../obj/$(SYSDIR)

# VPATH += $(OUTDIR)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
	cp $@ $(OUTDIR)

all: target

target: local subtarget

local: $(OBJS)

w:
	@echo -n

$(CODELIB): $(OBJS)
	$(AR) $(ARFLAGS) $(CODELIB) $(OBJS)
	cp $(CODELIB) ../lib/$(SYSDIR)

clean: subclean
	rm -f $(OUTDIR)/*.o *.o $(CODELIB) 

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


