include $(YARP_ROOT)/conf/Makefile.template

OBJS = $(patsubst %.cpp, %.o, $(wildcard *.cpp)) 
MAIN = porter

CFLAGS = $(CFAST) -I$(YARP_ROOT)/src/hardware/include/cpp -L$(YARP_ROOT)/src/hardware/lib -I$(YARP_ROOT)/include -L$(YARP_ROOT)/lib/$(SYSDIR)

all: $(MAIN)

$(MAIN): $(MAIN).o $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(YARP_ROOT)/lib/$(SYSDIR)/os_services.lib $(LIBADD)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -f *.o
	rm -f $(MAIN)

install: $(MAIN)
	$(CP) $(MAIN) $(YARP_ROOT)/bin/$(OS)/
