include $(YARP_ROOT)/conf/Makefile.template

NEWPATH = -I$(YARP_ROOT)/include -I$(YARP_ROOT)/include/stl -I$(YARP_ROOT)/include/sys -I$(YARP_ROOT)/include/conf -L$(YARP_ROOT)/lib/$(SYSDIR)
LIBPATH = $(YARP_ROOT)/lib/$(SYSDIR)

CFLAGS = $(CFAST) $(NEWPATH) $(CDEFS)
LIBS = $(LIBPATH)/images.lib $(LIBPATH)/fakeipl.lib $(LIBPATH)/os_services.lib $(LIBADD)

MAIN = cogcam_io

%.o: %.cpp
	$(CC) $(CFLAGS) $(CDEFS) -c $^ -o $@

$(MAIN): $(MAIN).o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	rm -f $(MAIN) $(MAIN).o *.o

install: $(MAIN)
	$(CP) $(MAIN) $(YARP_ROOT)/bin/$(OS)
