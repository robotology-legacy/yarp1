CC = cc
OS = qnx

NEWPATH = -I$(YARP_ROOT)/include -I$(YARP_ROOT)/include/stl -I$(YARP_ROOT)/include/sys -L$(YARP_ROOT)/lib/$(OS)
OLDPATH = -I$(YARPED_ROOT)/include/cpp -I$(YARPED_ROOT)/include -L$(YARPED_ROOT)/lib

CDEFS = -D__QNX__ -D__QNX4__

#CFLAGS = -oxt $(NEWPATH) $(OLDPATH) $(CDEFS)
CFLAGS = -g3 $(NEWPATH) $(OLDPATH) $(CDEFS)
LIBS = -lhmm.lib -limages.lib -lfakeipl.lib -lmath.lib -lmodels.lib -limgclient -lsom.lib

MAIN = testhmm

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

$(MAIN): $(MAIN).o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	rm -f $(MAIN) $(MAIN).o

install: $(MAIN)
	cp $(MAIN) $(YARPED_ROOT)/bin
