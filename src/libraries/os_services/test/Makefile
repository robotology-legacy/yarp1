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
EXEC_TARGETS = $(patsubst %.cpp, $(GENDIR)/%, $(wildcard exec_*.cpp))
CFLAGS = -g -I../include -I/home/cog/include/cpp -I/usr/include
#cpp is just there for the stl
LIBS = ../lib/$(SYSDIR)/os_services.lib $(SYSLIB)

SUBDIRS =

GENDIR = obj/$(SYSDIR)

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


