include $(YARP_ROOT)/conf/Makefile.template


NEWPATH = -I. -I$(YARP_ROOT)/include -I$(YARP_ROOT)/include/stl -I$(YARP_ROOT)/include/sys -L$(YARP_ROOT)/lib/$(OS)
HWINCLUDE = -I$(YARP_ROOT)/src/hardware/include -L$(YARP_ROOT)/src/hardware/lib/
LIBS = -lmath.lib -lmodels.lib -los_services.lib -lMotorCtrl.lib -lmeid.lib -lisd.lib -lueid.lib -lYAPOCmsg.lib -lK4JointPos.lib
CDEFS = -D__QNX__ -D__QNX4__

CFLAGS = -oxt $(NEWPATH) $(HWINCLUDE) $(OLDPATH) $(CDEFS)



MAIN = YAPOCClient

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

$(MAIN): $(MAIN).o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	rm -f $(MAIN) $(MAIN).o

install: $(MAIN)
	cp -t $(MAIN) $(YARP_ROOT)/bin/$(OS)

