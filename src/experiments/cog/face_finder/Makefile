include $(YARP_ROOT)/conf/Makefile.template

NEWPATH = -I$(YARP_ROOT)/include -I$(YARP_ROOT)/include/stl -I$(YARP_ROOT)/include/sys -L$(YARP_ROOT)/lib/$(OS)
OLDPATH = -I../../include

CDEFS = -D__QNX__ -D__QNX4__

CFLAGS = $(CFAST) $(NEWPATH) $(OLDPATH) $(CDEFS)
LIBS = -limages.lib -lfakeipl.lib -los_services

MAIN = face_finder

%.o: %.cpp
	$(CC) $(CFLAGS) $(CDEFS) -c $^ -o $@

$(MAIN): $(MAIN).o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	rm -f $(MAIN) $(MAIN).o

install: $(MAIN)
	cp -t $(MAIN) $(YARP_ROOT)/bin/$(OS)
