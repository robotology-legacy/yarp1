include $(YARP_ROOT)/conf/Makefile.template

LIBS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
CODELIB = fakeipl
OS = $(SYSDIR)

#CFLAGS = -g3 -I. -I$(YARP_ROOT)/include -I$(YARP_ROOT)/include/sys $(CDEFS)
CFLAGS = $(CFAST) $(CDEFS) -D__FAKEIPL__ -I. -I$(YARP_ROOT)/include -I$(YARP_ROOT)/include/sys

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
