include $(YARP_ROOT)/conf/Makefile.template

CC = cc
CFLAGS = -I$(YARP_ROOT)/src/hardware/include/cpp -L$(YARP_ROOT)/src/hardware/lib -D__QNX__
LIBS = 

MAIN = reset_isd

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

$(MAIN): $(MAIN).o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	rm -f $(MAIN) $(MAIN).o
