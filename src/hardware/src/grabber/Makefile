include $(YARP_ROOT)/conf/Makefile.template

PXC_VERSION = pxc2plus
#PXC_VERSION = pxc200

OBJS = $(patsubst %.cpp, %.o, $(wildcard *.cpp)) 
MAIN = grabber

CLINK = images.lib os_services.lib fakeipl.lib math.lib models.lib -l$(PXC_VERSION) -T1 -Wc,-w4
CFLAGS = $(CFAST) -D__QNX__ -D__QNX4__ -DPXC_VERSION_$(PXC_VERSION) -I$(YARP_ROOT)/include/stl -I$(YARP_ROOT)/include -I$(YARP_ROOT)/include/sys -L$(YARP_ROOT)/src/hardware/lib  -L$(YARP_ROOT)/lib/$(SYSDIR)

all: $(MAIN)

$(MAIN): $(MAIN).o $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(CLINK)
	usemsg $(MAIN) USAGE.TXT

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -f *.o
	rm -f $(MAIN)

install: $(MAIN)
	$(CP) $(MAIN) $(YARP_ROOT)/bin/$(OS)
