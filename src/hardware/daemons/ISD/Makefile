include $(YARP_ROOT)/conf/Makefile.template



all:  isd.lib isd

isd.lib:
	cd client; make

isd:
	cd daemon; make


install:  all
	$(CP) client/isd.lib $(YARP_ROOT)/src/hardware/lib
	chmod a+r $(YARP_ROOT)/src/hardware/lib/isd.lib
	$(CP) daemon/isd $(YARP_ROOT)/bin/$(OS)/
	chmod a+rx $(YARP_ROOT)/bin/$(OS)/isd
	$(CP) include/isd.h $(YARP_ROOT)/src/hardware/include
	chmod a+r $(YARP_ROOT)/src/hardware/include/isd.h

clean:
	+cd ./client; make $(@F)
	+cd ./daemon; make $(@F)
