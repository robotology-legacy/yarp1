include $(YARP_ROOT)/conf/Makefile.template

ifeq ($(shell uname),QNX)
  CC = cc $(CFAST)
  CC1 = $(CC)
  CDEFS = -D__QNX__ -D__QNX4__
  SYSDIR = qnx
  SYSLIB = 
  CP = cp -t
else
  ifeq ($(shell uname),Linux)
    CC = g++
    CC1 = gcc
    CDEFS = -D__LINUX__
    SYSDIR = linux
    SYSLIB = -lpthread
    CP = cp
  else
    CC = g++ 
    CC1 = gcc 
    CDEFS = -D__WIN__
    SYSDIR = winnt
    SYSLIB = 
    CP = cp
  endif
endif

LIBS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
CODELIB = fakeipl
OS = $(SYSDIR)

#CFLAGS = -g3 -I. -I$(YARP_ROOT)/include -I$(YARP_ROOT)/include/sys $(CDEFS)
CFLAGS = -I. $(CFAST) -I$(YARP_ROOT)/include -I$(YARP_ROOT)/include/sys $(CDEFS)

all: $(CODELIB).a

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

$(CODELIB).a: $(LIBS)
	$(AR) $(ARFLAGS) $(CODELIB).a $(LIBS)

clean:
	rm -f *.o
	rm -f $(CODELIB).a

install: $(CODELIB).a
	$(CP) $(CODELIB).a $(YARP_ROOT)/lib/$(OS)/$(CODELIB).lib
	$(CP) *.h $(YARP_ROOT)/include
#	cp -t *.inl $(YARP_ROOT)/include
