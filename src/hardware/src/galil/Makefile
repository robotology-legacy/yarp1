# Device driver global makefile
include $(YARP_ROOT)/conf/Makefile.template

all: compile 

clean:
	+cd ./$(OS); make $(@F)
	
compile:
	+cd .$(OS); make

install: 
	+cd ./$(OS); make install
	+cd ./common; make install
