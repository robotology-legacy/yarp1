ifeq ($(shell uname),QNX)
  CC = cc -l socket -l unix -D__QNX__ -D__QNX4__
  SYSDIR = qnx
  SYSLIB = 
else
  ifeq ($(shell uname),Linux)
    CC = g++ -D__LINUX__
    SYSDIR = linux
    SYSLIB = -lpthread
  else
    CC = g++ -D__WINNT__
    SYSDIR = winnt
    SYSLIB = 
  endif
endif

TARGETS = $(patsubst %.cpp, $(GENDIR)/%.o, $(wildcard *.cpp))
EXEC_TARGETS = $(patsubst %.cpp, $(GENDIR)/%, $(wildcard *.cpp))
CFLAGS = -g -I../include -I../../include -I$(YARP_ROOT)/include -I$(YARP_ROOT)/include/sys -I../include -I/home/cog/include/cpp -I/usr/include
#cpp is just there for the stl in qnx
LIBS = -L$(YARP_ROOT)/lib/$(SYSDIR) $(YARP_ROOT)/lib/$(SYSDIR)/speech_tts.lib -ldl -libmeci50 $(YARP_ROOT)/lib/$(SYSDIR)/os_services.lib $(SYSLIB)

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
	rm -f *_out.ppm

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


