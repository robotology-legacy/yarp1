include $(YARP_ROOT)/conf/Makefile.template


LIBS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
CODELIB = K4JointPos

CFLAGS = -oxt -I. -I$(YARP_ROOT)/include -D__QNX__

all: $(CODELIB).a

%.o: %.cpp
	cc $(CFLAGS) -c $^ -o $@

$(CODELIB).a: $(LIBS)
	$(AR) $(ARFLAGS) $(CODELIB).a $(LIBS)

clean:
	rm -f *.o
	rm -f $(CODELIB).a

install: $(CODELIB).a
	cp -t $(CODELIB).a $(YARP_ROOT)/lib/$(OS)/$(CODELIB).lib
	cp -t *.h $(YARP_ROOT)/include
	
