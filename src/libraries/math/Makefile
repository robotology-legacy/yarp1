include $(YARP_ROOT)/conf/Makefile.template

LIBS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
CODELIB = math
OS = $(SYSDIR)

CFLAGS = $(CFAST) -I. -I$(YARP_ROOT)/include $(CDEFS)

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
	$(CP) *.inl $(YARP_ROOT)/include
