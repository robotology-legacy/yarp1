include $(YARP_ROOT)/conf/Makefile.template




all:  ueid.lib ueid

ueid.lib:
	cd client; make

ueid:
	cd daemon; make

clean: 
	cd daemon; make clean

install:  all
	cp client/ueid.lib $(YARP_ROOT)/src/hardware/lib
	chmod a+r $(YARP_ROOT)/src/hardware/lib/ueid.lib
	cp daemon/ueid $(YARP_ROOT)/bin/$(OS)
	chmod a+rx $(YARP_ROOT)/bin/$(OS)/ueid
	cp include/ueid.h $(YARP_ROOT)/src/hardware/include
	chmod a+r $(YARP_ROOT)/src/hardware/include/ueid.h


