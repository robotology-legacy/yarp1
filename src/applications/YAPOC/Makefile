include $(YARP_ROOT)/conf/Makefile.template

NEWPATH = -I$(YARP_ROOT)/include -I$(YARP_ROOT)/include/stl -I$(YARP_ROOT)/include/sys -L$(YARP_ROOT)/lib/$(OS)
HWINCLUDE = -I$(YARP_ROOT)/src/hardware/include -L$(YARP_ROOT)/src/hardware/lib/


CDEFS = -D__QNX__ -D__QNX4__

CFLAGS = -oxt $(NEWPATH) $(HWINCLUDE) $(OLDPATH) $(CDEFS)
#CFLAGS = -g3 $(NEWPATH) $(HWINCLUDE) $(OLDPATH) $(CDEFS)
LIBS = -limages.lib -lfakeipl.lib -lmath.lib -lmodels.lib -los_services -lmeid.lib -lisd.lib -lueid.lib -lK4MotorCtrl.lib -lClientPIC.lib -lYAPOCmsg.lib -lK4JointPos.lib
#LIBS = -limages.lib -lfakeipl.lib -lmath.lib -lmodels.lib -los_services -lmeid.lib -lisd.lib -lueid.lib -lClientPIC.lib -lYAPOCmsg.lib -lK4JointPos.lib


MAIN = YAPOCControl

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

$(MAIN): $(MAIN).o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	rm -f $(MAIN) $(MAIN).o

install: $(MAIN)
	cp -t $(MAIN) $(YARP_ROOT)/bin/$(OS)
	cp -t *.h $(YARP_ROOT)/include
