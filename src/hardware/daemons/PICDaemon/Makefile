include $(YARP_ROOT)/conf/Makefile.template

OLDPATH = -I../../include

MYPATH = -L$(YARP_ROOT)/lib/$(OS)
CDEFS = -D__QNX__ -D__QNX4__

CFLAGS = -oxt $(CDEFS) $(MYPATH)

LIBS = -limages.lib -lfakeipl.lib -lmath.lib -lmodels.lib -los_services

MAIN = MainDaemonPIC

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

$(MAIN): $(MAIN).o q_sio_util.o q_controller.o DaemonPIC.o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@



clean:
	rm -f *.o
	rm -f $(MAIN)

install: $(MAIN)
	cp -t $(MAIN) $(YARP_ROOT)/bin/$(OS)		
	
