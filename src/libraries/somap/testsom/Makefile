CC = cc
CFLAGS = -g -I$(YARP_ROOT)/include -L$(YARP_ROOT)/lib/qnx -I$(YARPED_ROOT)/include/cpp -I$(YARPED_ROOT)/include -L$(YARPED_ROOT)/lib -D__QNX__
LIBS = -limgclient -limages.lib -lmath.lib -lmodels.lib -lSom.lib

MAIN = testsom

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

$(MAIN): $(MAIN).o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	rm -f $(MAIN) $(MAIN).o

install: $(MAIN)
	cp $(MAIN) $(YARPED_ROOT)/bin
