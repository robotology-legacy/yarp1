include $(YARP_ROOT)/conf/Makefile.template

NEWPATH = -I$(YARP_ROOT)/include -I$(YARP_ROOT)/include/stl -I$(YARP_ROOT)/include/sys -L$(YARP_ROOT)/lib/$(OS)

CDEFS = -D__QNX__ -D__QNX4__

CFLAGS = -oxt $(NEWPATH) $(OLDPATH) $(CDEFS) -N 128k
#CFLAGS = -g3 $(NEWPATH) $(OLDPATH) $(CDEFS)
LIBS = -limages.lib -lfakeipl.lib -lmath.lib -lmodels.lib -los_services

MAIN = camcal

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

$(MAIN): $(MAIN).o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	rm -f $(MAIN) $(MAIN).o

install: $(MAIN)
	cp -t $(MAIN) $(YARP_ROOT)/bin/$(OS)
