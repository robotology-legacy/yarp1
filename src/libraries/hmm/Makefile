LIBS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
CODELIB = hmm
OS = qnx
CDEFS = -D__QNX__ -D__QNX4__

#CFLAGS = -oxt -I. -I$(YARP_ROOT)/include $(CDEFS)
CFLAGS = -g3 -I. -I$(YARP_ROOT)/include $(CDEFS)

all: $(CODELIB).a

%.o: %.cpp
	cc $(CFLAGS) -c $^ -o $@

$(CODELIB).a: $(LIBS)
	$(AR) $(ARFLAGS) $(CODELIB).a $(LIBS)

clean:
	rm -f *.o
	rm -f $(CODELIB).a

install: $(CODELIB).a
	cp $(CODELIB).a $(YARP_ROOT)/lib/$(OS)/$(CODELIB).lib
	cp *.h $(YARP_ROOT)/include
#	cp *.inl $(YARP_ROOT)/include
