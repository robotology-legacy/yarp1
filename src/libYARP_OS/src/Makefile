
ifndef  YARP_ROOT
YARP_ROOT = ../../..
endif

include $(YARP_ROOT)/conf/Makefile.template

TARGETS = $(patsubst %.cpp, $(GENDIR)/%.o, $(wildcard *.cpp)) $(patsubst %.c, $(GENDIR)/%.o, $(wildcard *.c))

CFLAGS = -D_REENTRANT $(CFAST) $(CDEFS) -I../include -I../include/$(SYSDIR)

#CFLAGS = $(CFAST) $(CDEFS) -I../include/yarp -I../include/yarp_private -I../include/$(SYSDIR) -I../include

SUBDIRS =

GENDIR = ../obj/$(SYSDIR)

# library suffix should actually be system dependent
CODELIB = libYARP_OS.a

LIBGENDIR = ../lib/$(SYSDIR)

VPATH += $(GENDIR)

all: target

target: $(TARGETS) $(LIBGENDIR)/$(CODELIB) subtarget

w:
	@echo -n

$(GENDIR)/%.o: %.cpp
	mkdir -p $(GENDIR)
	$(CC) $(CFLAGS) -c $^ -o $@

$(GENDIR)/%.o: %.c
	mkdir -p $(GENDIR)
	$(CC1) $(CFLAGS) -c $^ -o $@


$(LIBGENDIR)/$(CODELIB): $(TARGETS)
	mkdir -p $(LIBGENDIR)
	echo Making $(CODELIB) for $(TARGETS)
	$(AR) $(ARFLAGS) $(LIBGENDIR)/$(CODELIB) $(TARGETS)

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


