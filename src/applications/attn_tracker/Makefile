include $(YARP_ROOT)/conf/Makefile.template

NEWPATH = -I$(YARP_ROOT)/include -I$(YARP_ROOT)/include/stl -I$(YARP_ROOT)/include/sys -L$(YARP_ROOT)/lib/$(OS) -I$(YARP_ROOT)/include/conf
OLDPATH = -I../../include

CFLAGS = $(CFAST) $(NEWPATH) $(OLDPATH) $(CDEFS)
LIBS = $(YARP_ROOT)/src/hardware/lib/meid.lib  -lmath $(YARP_ROOT)/src/hardware/lib/ueid.lib -lMotorCtrl -limages.lib -lfakeipl.lib -los_services
MAIN = attn_tracker

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

$(MAIN): $(MAIN).o ImgTrack.o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	rm -f $(MAIN) $(MAIN).o *.o

install: $(MAIN)
	cp -t $(MAIN) $(YARP_ROOT)/bin/$(OS)
