# Device driver global makefile
include $(YARP_ROOT)/conf/Makefile.template

all: drivers

clean:
	+cd ./picolo/$(OS); make $(@F)
	+cd ./galil/$(OS); make $(@F)

install: drivers

drivers: 
	+cd ./picolo/$(OS); make install
	+cd ./galil/$(OS); make install
	+cd ./common; make install
