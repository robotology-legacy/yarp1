# Device driver global makefile
include $(YARP_ROOT)/conf/Makefile.template

all: compile drivers

clean:
	+cd ./picolo/$(OS); make $(@F)
	+cd ./galil/$(OS); make $(@F)

install: drivers

compile:
	+cd ./picolo/$(OS); make
	+cd ./galil/$(OS); make

drivers: 
	+cd ./picolo/$(OS); make install
	+cd ./galil/$(OS); make install
	+cd ./common; make install
